# -*- coding:utf-8 -*-
#
# Wrapping Column 132.
import scrapy
import json
from otcconfigure import *

# for signals in scarpy, such as dispatcher.connect(self.initialize, signals.engine_started) 
from scrapy import signals
from scrapy.xlib.pydispatch import dispatcher

import MySQLdb
import datetime

import time

# the view way
'''
select d.*
  from stock_detail d,
    (select stock_id, max(line_number) as ts from stock_detail group by stock_id) m
  where d.stock_id = m.stock_id and d.line_number = m.ts



in fetchall
(13L, '63.ah', '800015.ah', '\xe7\xa5\x9e\xe8\x99\xb9\xe8\x82\xa1\xe4\xbb\xbd', '\xe5\xae\x89\xe5\xbe\xbd\xe7\x9c\x81\xe7\xa5\x9e\xe8\x99\xb9\xe5\x8f\x98\xe5\x8e\x8b\xe5\x99\xa8\xe8\x82\xa1\xe4\xbb\xbd\xe6\x9c\x89\xe9\x99\x90\xe5\x85\xac\xe5\x8f\xb8', '', '2013-09-30', 'http://www.ahsgq.com/aee/bm/bminfo.jsp?id=63', datetime.datetime(2015, 10, 23, 12, 33, 31)) <type 'tuple'>
'''

def GeneratorInsertHead(columndictall):
    columnvalue = columndictall[POS_COLUMN].values()
    sql_insert = 'insert into '+columndictall[POS_TABLE]+'('+','.join(columnvalue)+') values '
    return sql_insert

def GeneratorUsefulColumn(columndictall, alias):
    columndict = columndictall[POS_COLUMN]
    columnvalue = []
    if alias:
        alias = alias + '.'
    for key in columndict:
        if key != UPDATETIME:
            columnvalue.append(alias+columndict[key])
    return columnvalue

def GeneratorSelectHead(columndictall, alias):
    columnvalue = GeneratorUsefulColumn(columndictall, alias)
    return 'select ' + ','.join(columnvalue) + ' from '

def GeneratorOneRecord(stockinfo, columndict):
    onerecord = []
    for onekey in columndict.keys():
        isnumber = onekey in DATATYPEDICT['int'] or onekey in DATATYPEDICT['float']
        if isnumber:
            onerecord.append(str(stockinfo.get(onekey, 0)))
        else:
            onerecord.append('\''+stockinfo.get(onekey, '')+'\'')
    return '('+','.join(onerecord)+')'

def GeneratorInsert(allstockinfo, columndictall):
    inserthead = GeneratorInsertHead(columndictall)
    nowtime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    records = []
    for onestockinfo in allstockinfo:
        onestockinfo[UPDATETIME] = nowtime
        records.append(GeneratorOneRecord(onestockinfo, columndictall[POS_COLUMN]))
    return inserthead + ','.join(records) + ';'

def GeneratorNewest(columndictall, alias):
    sql_select = '{table} {alias}, (select {sid}, max(line_number) as ts from {table} group by {sid}) viewalias where {alias}.{sid}=viewalias.{sid} and {alias}.line_number=viewalias.ts'.format(table=columndictall[POS_TABLE], sid=columndictall[POS_COLUMN][STOCKID], alias=alias)
    return sql_select

def GeneratorSelectNewest(columndictall):
    return GeneratorSelectHead(columndictall, 'd') + GeneratorNewest(columndictall, 'd')


def InsertQuotes(allstockinfo, globalinfo):
    try:
        conn=MySQLdb.connect(host='192.168.206.139',user='root',passwd='',db='mysql',port=3306)
        cur=conn.cursor()
# found this line for two hours, useful in http://www.cnblogs.com/discuss/articles/1862248.html
        cur.execute('SET NAMES "utf8";')

        allsql = GeneratorInsert(allstockinfo, globalinfo[DATABASEQUOTATION])
        cur.execute(allsql)

        if TOTAL_CRAWL:
            allsql = GeneratorInsert(allstockinfo, globalinfo[DATABASEDETAIL])
            cur.execute(allsql)

        conn.commit()
        cur.close()
        conn.close()
    except MySQLdb.Error,e:
        print "Mysql Error %d: %s" % (e.args[0], e.args[1])

def GetNewestRecord(cur, globalinfo):
    try:
        sql = GeneratorSelectNewest(globalinfo)
        cur.execute(sql)
        stockdict = {}
        usefulcolumn = GeneratorUsefulColumn(globalinfo, '')

        for onerow in cur.fetchall():
            onestockdict = {}
            sequ = 0
            for onecol in usefulcolumn:
                onestockdict[onecol] = onerow[sequ]
#                print onecol, onestockdict[onecol]
                sequ += 1
            stockid = globalinfo[POS_COLUMN][STOCKID]
            stockdict[onestockdict[stockid]] = onestockdict
    except MySQLdb.Error,e:
        print "Mysql Error %d: %s" % (e.args[0], e.args[1])
    return stockdict

def OpenMySQL(connstr):
    try:
        conn = MySQLdb.connect(host=connstr.get(MYSQL_HOST), user=connstr.get(MYSQL_USER, 'root'), passwd=connstr.get(MYSQL_PASSWD, ''), db=connstr.get(MYSQL_DB, 'mysql'), port=connstr.get(MYSQL_PORT, 3306))
    except MySQLdb.Error,e:
        print "Mysql Error %d: %s" % (e.args[0], e.args[1])
        return None
    return conn

def OpenCursor(conn):
    try:    
        cur=conn.cursor()
        cur.execute('SET NAMES "utf8";')
    except MySQLdb.Error,e:
        print "Mysql Error %d: %s" % (e.args[0], e.args[1])
        return None
    return cur

def CloseMySQL(oneobject):
    try:
        oneobject.close()
    except MySQLdb.Error,e:
        print "Mysql Error %d: %s" % (e.args[0], e.args[1])

def IsRecordChange(newrecord, originrecord, columndict):
    for onecol in columndict:
        if onecol in newrecord.keys() and newrecord[onecol] != originrecord[columndict[onecol]]:
            return True
    return False

def CompareRecord(allstockinfo, recordlist, globalinfo):
    shouldupdate = []
    for onestock in allstockinfo:
#        print 'onestock', onestock

        #datafield = globalinfo[onestock]
        stockid = onestock[STOCKID]
        if not stockid in recordlist:
            shouldupdate.append(onestock)
            print 'add append in no stock', stockid
        elif IsRecordChange(onestock, recordlist[stockid], globalinfo[POS_COLUMN]):
            shouldupdate.append(onestock)
            print 'add append for change', stockid
    return shouldupdate

def UpdateRecordIfNew(cur, globalinfo):
    newrecord = GetNewestRecord(cur, globalinfo)
    shouldupdate = CompareRecord(TotalStock, newrecord, globalinfo)
    if shouldupdate:
        allsql = GeneratorInsert(shouldupdate, globalinfo)
        cur.execute(allsql)

if __name__ == '__main__':
    MySQLConnect = OpenMySQL(MYSQL_CONNECT)
    MySQLCursor = OpenCursor(MySQLConnect)

    print type(MySQLConnect), type(MySQLCursor)

HTTP_OK                         = 200

def GetStockDetail(stockinfo, infodict, response):              # here stockinfo is ref para
    for oneinfokey in infodict.keys():
        if oneinfokey:
            oneinforesult = GetStringByXpath(infodict[oneinfokey], response)
            if oneinfokey in DATATYPEDICT['int']:
                try:
                    stockinfo[oneinfokey] = int(oneinforesult)
                except:
                    stockinfo[oneinfokey] = int(0)
            elif oneinfokey in DATATYPEDICT['float']:
                try:
                    stockinfo[oneinfokey] = float(oneinforesult)
                except:
                    stockinfo[oneinfokey] = float(0)
            else:
                stockinfo[oneinfokey] = oneinforesult
        else:
            oneinforesult = ''
            stockinfo[oneinfokey] = oneinforesult

def GetNumberByFind(key, result):
    if isinstance(key, int):
        return key
    elif isinstance(key, str) or isinstance(key, unicode):
        place = result.find(key)
        if place != -1:
            return place
        else:
            raise

def GetStringByXpath(xpathkey, response):
    if isinstance(xpathkey, list):
        xpathkeynumber = len(xpathkey)
        if xpathkeynumber >= 1:
            realxpathkey = xpathkey[0]
        else:
            return ''
    elif isinstance(xpathkey, int):
        return str(xpathkey)
    elif isinstance(xpathkey, str) or isinstance(xpath, unicode):
        xpathkeynumber = 0
        realxpathkey = xpathkey
    else:
        return ''
    if not realxpathkey:
        return ''

    try:
        xpathresult = response.xpath(realxpathkey).extract()[0].strip().encode('utf-8')
    except:
        return ''
    if xpathkeynumber > POS_START:
        resultstart = GetNumberByFind(xpathkey[1], xpathresult) + len(xpathkey[1])
    else:
        resultstart = 0
    if xpathkeynumber > POS_END:
        resultend = GetNumberByFind(xpathkey[2], xpathresult[resultstart:]) + resultstart
    else:
        resultend = len(xpathresult)
    return xpathresult[resultstart : resultend]
    
def DisplayStockInfo(totalstock):
    for onestock in totalstock:
        for oneattr in onestock.keys():
            print oneattr + ' : ' + str(onestock[oneattr]) + ',\t',
        print ''
    print 'total : ', len(totalstock)

def JoinDictionary(localdict, globaldict):
    for onekey in globaldict:
        if not onekey in localdict:
            localdict[onekey] = globaldict[onekey]
        elif isinstance(localdict[onekey], dict) and isinstance(globaldict[onekey], dict):
            JoinDictionary(localdict[onekey], globaldict[onekey])

def FormatStockInfo(oneotc, stockinfo, response, displayformat):
    if not displayformat:
        return stockinfo

    newinfo = stockinfo
    for onedetail in displayformat.keys():
        displayparaplist = []
        oneformat = displayformat[onedetail]

        if oneformat.find('{STOCKNUMBER}') != -1:
            displayparaplist.append('STOCKNUMBER=stockinfo[STOCKNUMBER]')
        if oneformat.find('{ABBRNAME}') != -1:
            displayparaplist.append('ABBRNAME=oneotc[ABBRNAME]')
        if oneformat.find('{STOCKURL}') != -1:
            displayparaplist.append('STOCKURL=response.url')
        if oneformat.find('{ONESTOCKID}') != -1:
            displayparaplist.append('ONESTOCKID=stockinfo[ONESTOCKID]')

        displayexec = 'newinfo[onedetail] = oneformat.format(' + ','.join(displayparaplist) + ')'
        exec(displayexec)
    return newinfo

TotalStock = []

class AllQuotes(scrapy.Spider):
    name = 'otcquotes'
    allowed_domains = []
    HTTPERROR_ALLOW_ALL = True

    def __init__(self):
        dispatcher.connect(self.initialize, signals.engine_started)
        dispatcher.connect(self.finalize, signals.engine_stopped)

    def initialize(self):
        pass
 
    def finalize(self):
        self.MySQLConnect = OpenMySQL(MYSQL_CONNECT)
        self.MySQLCursor = OpenCursor(self.MySQLConnect)

        UpdateRecordIfNew(self.MySQLCursor, GLOBAL_INFO[DATABASEQUOTATION])
        if TOTAL_CRAWL:
            UpdateRecordIfNew(self.MySQLCursor, GLOBAL_INFO[DATABASEDETAIL])

        self.MySQLConnect.commit()

        CloseMySQL(self.MySQLCursor)
        CloseMySQL(self.MySQLConnect)

 
    def start_requests(self):
        for oneotc in OTC_SITES:
            JoinDictionary(oneotc, GLOBAL_INFO)
            request = scrapy.Request(oneotc[HOMEPAGE_URL], self.parse_start)
            request.meta['oneotc'] = oneotc
            yield request

    def parse_start(self, response):
        status = response.status
        if status != HTTP_OK:
            print 'Error in get %s, httpstatus : %d' %(response.url, status)
            return

        oneotc = response.meta['oneotc']
        pagenumber = GetStringByXpath(oneotc[PAGESTART], response)
        if pagenumber:
            pagestart = int(pagenumber)
        else:
            return
        pagenumber = GetStringByXpath(oneotc[PAGEEND], response)
        if pagenumber:
# range(1,2) only run once
            pageend = int(pagenumber) + 1
        else:
            return

        pageparalist = []
        if not ONEPAGE_URL in oneotc.keys() or not oneotc[ONEPAGE_URL]:
            return
        quotesurl = oneotc[ONEPAGE_URL]
        if quotesurl.find('{pagenum}') != -1:
            pageparalist.append('pagenum=onepage')
        pageurl = "onepageurl = quotesurl.format(" + ",".join(pageparalist) + ")"

        for onepage in range(pagestart, pageend):
# onepageurl now is the url, should be send as request
            exec(pageurl)
            request = scrapy.Request(onepageurl, self.parse_list)
            request.meta['oneotc'] = oneotc
            yield request

    def parse_list(self, response):
        status = response.status
        if status != HTTP_OK:
            print 'Error in get %s, httpstatus : %d' %(response.url, status)
            return
        oneotc = response.meta['oneotc']
        if not ONESTOCK_FORMAT in oneotc.keys() or not oneotc[ONESTOCK_FORMAT]:
            return

        for onestockinlist in response.xpath(oneotc[ONESTOCK_FORMAT]):   
            onestockid = GetStringByXpath(oneotc[ONESTOCKID], onestockinlist)
            if not onestockid:
                continue

            stockinfo = {}
            stockinfo[ONESTOCKID] = onestockid
            GetStockDetail(stockinfo, oneotc[LISTPAGEDETAIL], onestockinlist)

            if not TOTAL_CRAWL:
# stock detail info should not crawl everytime
                formatedstockinfo = FormatStockInfo(oneotc, stockinfo, response, oneotc[DISPLAYINFOPRICE])
                TotalStock.append(formatedstockinfo)
                continue

            stockparalist = []
            if not DETAILSTOCK_URL in oneotc.keys() or not oneotc[DETAILSTOCK_URL]:
                continue
            stockdetail = oneotc[DETAILSTOCK_URL]
            if stockdetail.find('{stockid}') != -1:
                stockparalist.append('stockid=onestockid')
            stockurl = 'onestockurl = stockdetail.format(' + ','.join(stockparalist) + ')'
            exec(stockurl)

            request = scrapy.Request(onestockurl, self.parse_stock)
            request.meta['oneotc'] = oneotc
            request.meta['info'] = stockinfo
            yield request

    def parse_stock(self, response):
        status = response.status
        if status != HTTP_OK:
            print 'Error in get %s, httpstatus : %d' %(response.url, status)
            return
        oneotc = response.meta['oneotc']
        stockinfo = response.meta['info']

        if not DETAILSTOCK_FORMAT in oneotc.keys() or not oneotc[DETAILSTOCK_FORMAT]:
            onestockdetail = response
        else:
            onestockdetail = response.xpath(oneotc[DETAILSTOCK_FORMAT])
        GetStockDetail(stockinfo, oneotc[STOCKPAGEDETAIL], onestockdetail)

        if stockinfo[STOCKNUMBER]:
            formatedstockinfo = FormatStockInfo(oneotc, stockinfo, response, oneotc[DISPLAYINFODETAIL])
            TotalStock.append(formatedstockinfo)


'''
CREATE TABLE `quotes`.`stock_quotation` (
  `line_number` INT NOT NULL AUTO_INCREMENT,
  `stock_id` VARCHAR(32) NOT NULL,
  `stock_price` FLOAT NULL DEFAULT 0,
  `stock_totalamount` INT NULL DEFAULT 0,
  `stock_todayamount` INT NULL DEFAULT 0,
  `update_time` DATETIME NOT NULL,
  PRIMARY KEY (`line_number`));
'''
