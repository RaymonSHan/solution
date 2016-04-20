# -*- coding:utf-8 -*-

import string

CLASS_NATURAL_PERSON            = 0x0001
CLASS_LEGAL_PERSON              = 0x0002
CLASS_PROCEDURE_PERSON          = 0x0003

CLASS_SECURITY_ACCOUNT          = 0x1001
CLASS_FONDING_ACCOUNT           = 0x1002
CLASS_BANK_ACCOUNT              = 0x1003

CLASS_PRIVATE_STOCK             = 0x5300

ID_SEX_MALE                     = 0x100001
ID_SEX_FEMALE                   = 0x100002

ID_CERTIFICATE_CARD             = 0x200001
ID_CERTIFICATE_PASSPORT         = 0x200002
ID_CERTIFICATE_ORGANIZATION     = 0x200101
ID_CERTIFICATE_TAX              = 0x200102
ID_CERTIFICATE_COMMERCIAL       = 0x200103

ID_MARKET_INTEROTC              = 0x300001
ID_MARKET_OTCCOMMON             = 0x300002

ID_TYPE_PRIMARY                 = 0x400001
ID_TYPE_SECONDRY                = 0x400002
ID_TYPE_NOMINAL                 = 0x401003

RESULT_NOTFOUND                 = (0, 0)
RESULT_ERR                      = (1, 0)

################################################################################

SEQU_HOLDER_DEFAULT             = 0xEA000001

TABLE_SHAREBALANCE              = "ShareBalance"
FIELD_TOTALAMOUNT               = "balTotalAmount"
FIELD_IPO                       = "balIPO"

def AnalyzePersonIdentity(valcert, valname):
  if string.atoi(valcert[16:17]) % 2 == 0:
    valsex = ID_SEX_FEMALE
  else:
    valsex = ID_SEX_MALE
  valbirth = '-'.join((valcert[6:10], valcert[10:12], valcert[12:14]))
  return (valsex, valbirth)
