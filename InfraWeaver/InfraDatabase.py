# -*- coding:utf-8 -*-

import mysql.connector
from InfraCommon import *

class InfraDatabase(object): 
  """This is Database connect and query"""
  
  def __init__(self, username, password, hostip):
    try:
      self.ConnectSQL = mysql.connector.connect(user=username, password=password, host=hostip, database='InfraWeaver')
    except Exception as e:  
      print(e)
  def __del__(self):
    try:
      self.ConnectSQL.close()
    except Exception as e:
      print(e)

  def __GetCursor(self):
    return self.ConnectSQL.cursor()
  def __Execute(self, procedure, parameter):
    try:
      result = None
      executecursor = self.__GetCursor()
      executecursor.callproc(procedure, parameter)
      for executeresult in executecursor.stored_results():
        result = executeresult.fetchone()
    except Exception as e:
      print(e)
    finally:
      executecursor.close()
    return result
  def __Query(self, procedure, parameter):
    try:
      executecursor = self.__GetCursor()
      executecursor.callproc(procedure, parameter)
      result = []
      for executeresult in executecursor.stored_results():
        result.append( executeresult.fetchall() )
    except Exception as e:
      print(e)
    finally:
      executecursor.close()
    return result

  def __ExecuteAdd(self, addproc, addpara):
    resultset = self.__Execute(addproc, addpara)
    if resultset == None:
      return RESULT_ERR  # first 1 for error in add
    else:
      return resultset
  def __ExecuteGet(self, getproc, getpara):
    resultset = self.__Execute(getproc, getpara)
    if resultset == None:
      return RESULT_NOTFOUND  # first 0 for success, second 0 for not found
    else:
      return resultset
  def __ExecuteReturn(self, getproc, getpara, addproc, addpara):
    (result, returnid) = self.__ExecuteGet(getproc, getpara)
    if result == 0 and returnid == 0:
      (result, returnid) = self.__ExecuteAdd(addproc, addpara)
    return (result, returnid)

  def __AddHolder(self, procper, paraper, proccert, paracert, procacc, paraacc):
    (resultc, sequcert) = self.__ExecuteGet("GetBaseCertificate", paracert)
    if sequcert == 0:
      (resultc, sequcert) = self.__ExecuteAdd(proccert, paracert)
      if resultc != 0:
        return RESULT_ERR
      (resultp, sequper) = self.__ExecuteAdd(procper, paraper)
      if resultp != 0:
        return RESULT_ERR
    else:
      (resultp, sequper) = self.__ExecuteGet("GetPersonBySequcert", (sequcert,))
      if resultp != 0 or sequper == 0:
        return RESULT_ERR
      print "sequper", sequper
    (resulta, sequacc) = self.__ExecuteAdd(procacc, paraacc)
    if resulta != 0:
      return RESULT_ERR
    return self.__ExecuteAdd("AddBaseHolder", (sequper, sequcert, sequacc))

  def AddAccountByIdentity(self, valcert, valname, valaccount, idmarket, idtype):
    paraper = AnalyzePersonIdentity(valcert, valname)
    paracert = (ID_CERTIFICATE_CARD, valcert, valname)
    paraacc = (idmarket, valaccount, idtype)
    return self.__AddHolder("AddNaturalPerson", paraper, "AddIdentityCard", paracert, "AddSecurityAccount", paraacc)

  def GetPrimaryHolderByIdentity(self, valcert, idmarket):
    getpara = (ID_CERTIFICATE_CARD, valcert, idmarket, "")
    return self.__ExecuteGet("GetPrimaryHolderByCert", getpara)

  def AddPrivateProdureSimple(self, valname, valcode, idmarket, vallimit):
    addpara = (CLASS_PRIVATE_STOCK, valname, "", valcode, idmarket, 0, vallimit)
    return self.__ExecuteAdd("AddPrivateProdure", addpara)

  def GetPrivateProdure(self, valcode, idmarket):
    getpara = (idmarket, valcode)
    return self.__ExecuteGet("GetPrivateProdure", getpara)

  def AddOneDocument(self, sequuser, descdoc, signdoc, docdetails):
    addpara = (sequuser, descdoc, signdoc)
    (result, sequdoc) = self.__ExecuteAdd("AddDocumentMain", addpara)
    if sequdoc == 0:
      return RESULT_ERR
    orderdoc = 0
    for onedetail in docdetails:
      (sequhold, sequprod, tabname, fiename, deltabal) = onedetail
      getpara = (sequhold, sequprod, tabname, fiename)
      result = self.__Execute("GetBalance", getpara)
      if result == None:
        bal = 0
        isnew = 1
      else:
        (resultget, bal) = result
        isnew = 0
      detailpara = (sequdoc, orderdoc, sequhold, sequprod, tabname, fiename, deltabal, bal, isnew)
      resultdetail = self.__Execute("ReplaceDocumentDetail", detailpara)
      if resultdetail == None:
        return RESULT_ERR
      orderdoc += 1

################################################################################

  def PrivateProdureRegister(self, sequuser, descdoc, valcode, idmarket, balance):
    (result, sequprod) = self.GetPrivateProdure(valcode, idmarket)
    if sequprod == 0:
        return RESULT_ERR
    detaild = (SEQU_HOLDER_DEFAULT, sequprod, TABLE_SHAREBALANCE, FIELD_TOTALAMOUNT + "_D", balance)
    detailc = (SEQU_HOLDER_DEFAULT, sequprod, TABLE_SHAREBALANCE, FIELD_IPO + "_C", balance)
    signdoc = "SIGNDOC"
    return self.AddOneDocument(sequuser, descdoc, signdoc, [detaild, detailc])

