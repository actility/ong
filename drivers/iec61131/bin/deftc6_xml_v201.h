/*
 *	 objet        : ppx:position
 *	 nb attr      : 0
 */
#define ppx:position.name$               ( ppx:position + 1 ) /* XoString */
#define ppx:position.xmlns               ( ppx:position + 2 ) /* XoString */
#define ppx:position.xmlns:xsd           ( ppx:position + 3 ) /* XoString */
#define ppx:position.xmlns:xsi           ( ppx:position + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonData
 *	 nb attr      : 2
 */
#define ppx:AnonData.name$               ( ppx:AnonData + 1 ) /* XoString */
#define ppx:AnonData.xmlns               ( ppx:AnonData + 2 ) /* XoString */
#define ppx:AnonData.xmlns:xsd           ( ppx:AnonData + 3 ) /* XoString */
#define ppx:AnonData.xmlns:xsi           ( ppx:AnonData + 4 ) /* XoString */
#define ppx:AnonData.name                ( ppx:AnonData + 5 ) /* XoString */
#define ppx:AnonData.handleUnknown       ( ppx:AnonData + 6 ) /* XoString */


/*
 *	 objet        : ppx:addData
 *	 nb attr      : 1
 */
#define ppx:addData.name$                ( ppx:addData + 1 ) /* XoString */
#define ppx:addData.xmlns                ( ppx:addData + 2 ) /* XoString */
#define ppx:addData.xmlns:xsd            ( ppx:addData + 3 ) /* XoString */
#define ppx:addData.xmlns:xsi            ( ppx:addData + 4 ) /* XoString */
#define ppx:addData.ppx:data             ( ppx:addData + 5 ) /* ppx:AnonData */


/*
 *	 objet        : ppx:connection
 *	 nb attr      : 5
 */
#define ppx:connection.name$             ( ppx:connection + 1 ) /* XoString */
#define ppx:connection.xmlns             ( ppx:connection + 2 ) /* XoString */
#define ppx:connection.xmlns:xsd         ( ppx:connection + 3 ) /* XoString */
#define ppx:connection.xmlns:xsi         ( ppx:connection + 4 ) /* XoString */
#define ppx:connection.globalId          ( ppx:connection + 5 ) /* XoString */
#define ppx:connection.refLocalId        ( ppx:connection + 6 ) /* XoString */
#define ppx:connection.formalParameter   ( ppx:connection + 7 ) /* XoString */
#define ppx:connection.ppx:position      ( ppx:connection + 8 ) /* ppx:position */
#define ppx:connection.ppx:addData       ( ppx:connection + 9 ) /* ppx:addData */


/*
 *	 objet        : ppx:rangeSigned
 *	 nb attr      : 0
 */
#define ppx:rangeSigned.name$            ( ppx:rangeSigned + 1 ) /* XoString */
#define ppx:rangeSigned.xmlns            ( ppx:rangeSigned + 2 ) /* XoString */
#define ppx:rangeSigned.xmlns:xsd        ( ppx:rangeSigned + 3 ) /* XoString */
#define ppx:rangeSigned.xmlns:xsi        ( ppx:rangeSigned + 4 ) /* XoString */


/*
 *	 objet        : ppx:connectionPointOut
 *	 nb attr      : 4
 */
#define ppx:connectionPointOut.name$     ( ppx:connectionPointOut + 1 ) /* XoString */
#define ppx:connectionPointOut.xmlns     ( ppx:connectionPointOut + 2 ) /* XoString */
#define ppx:connectionPointOut.xmlns:xsd ( ppx:connectionPointOut + 3 ) /* XoString */
#define ppx:connectionPointOut.xmlns:xsi ( ppx:connectionPointOut + 4 ) /* XoString */
#define ppx:connectionPointOut.globalId  ( ppx:connectionPointOut + 5 ) /* XoString */
#define ppx:connectionPointOut.ppx:relPo ( ppx:connectionPointOut + 6 ) /* ppx:position */
#define ppx:connectionPointOut.ppx:expre ( ppx:connectionPointOut + 7 ) /* XoString */
#define ppx:connectionPointOut.ppx:addDa ( ppx:connectionPointOut + 8 ) /* ppx:addData */


/*
 *	 objet        : ppx:connectionPointIn
 *	 nb attr      : 5
 */
#define ppx:connectionPointIn.name$      ( ppx:connectionPointIn + 1 ) /* XoString */
#define ppx:connectionPointIn.xmlns      ( ppx:connectionPointIn + 2 ) /* XoString */
#define ppx:connectionPointIn.xmlns:xsd  ( ppx:connectionPointIn + 3 ) /* XoString */
#define ppx:connectionPointIn.xmlns:xsi  ( ppx:connectionPointIn + 4 ) /* XoString */
#define ppx:connectionPointIn.globalId   ( ppx:connectionPointIn + 5 ) /* XoString */
#define ppx:connectionPointIn.ppx:relPos ( ppx:connectionPointIn + 6 ) /* ppx:position */
#define ppx:connectionPointIn.ppx:connec ( ppx:connectionPointIn + 7 ) /* ppx:connection */
#define ppx:connectionPointIn.ppx:expres ( ppx:connectionPointIn + 8 ) /* XoString */
#define ppx:connectionPointIn.ppx:addDat ( ppx:connectionPointIn + 9 ) /* ppx:addData */


/*
 *	 objet        : ppx:formattedText
 *	 nb attr      : 0
 */
#define ppx:formattedText.name$          ( ppx:formattedText + 1 ) /* XoString */
#define ppx:formattedText.xmlns          ( ppx:formattedText + 2 ) /* XoString */
#define ppx:formattedText.xmlns:xsd      ( ppx:formattedText + 3 ) /* XoString */
#define ppx:formattedText.xmlns:xsi      ( ppx:formattedText + 4 ) /* XoString */


/*
 *	 objet        : ppx:pouInstance
 *	 nb attr      : 5
 */
#define ppx:pouInstance.name$            ( ppx:pouInstance + 1 ) /* XoString */
#define ppx:pouInstance.xmlns            ( ppx:pouInstance + 2 ) /* XoString */
#define ppx:pouInstance.xmlns:xsd        ( ppx:pouInstance + 3 ) /* XoString */
#define ppx:pouInstance.xmlns:xsi        ( ppx:pouInstance + 4 ) /* XoString */
#define ppx:pouInstance.name             ( ppx:pouInstance + 5 ) /* XoString */
#define ppx:pouInstance.typeName         ( ppx:pouInstance + 6 ) /* XoString */
#define ppx:pouInstance.globalId         ( ppx:pouInstance + 7 ) /* XoString */
#define ppx:pouInstance.ppx:addData      ( ppx:pouInstance + 8 ) /* ppx:addData */
#define ppx:pouInstance.ppx:documentatio ( ppx:pouInstance + 9 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:AnonInfo
 *	 nb attr      : 4
 */
#define ppx:AnonInfo.name$               ( ppx:AnonInfo + 1 ) /* XoString */
#define ppx:AnonInfo.xmlns               ( ppx:AnonInfo + 2 ) /* XoString */
#define ppx:AnonInfo.xmlns:xsd           ( ppx:AnonInfo + 3 ) /* XoString */
#define ppx:AnonInfo.xmlns:xsi           ( ppx:AnonInfo + 4 ) /* XoString */
#define ppx:AnonInfo.name                ( ppx:AnonInfo + 5 ) /* XoString */
#define ppx:AnonInfo.version             ( ppx:AnonInfo + 6 ) /* XoString */
#define ppx:AnonInfo.vendor              ( ppx:AnonInfo + 7 ) /* XoString */
#define ppx:AnonInfo.ppx:description     ( ppx:AnonInfo + 8 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:addDataInfo
 *	 nb attr      : 1
 */
#define ppx:addDataInfo.name$            ( ppx:addDataInfo + 1 ) /* XoString */
#define ppx:addDataInfo.xmlns            ( ppx:addDataInfo + 2 ) /* XoString */
#define ppx:addDataInfo.xmlns:xsd        ( ppx:addDataInfo + 3 ) /* XoString */
#define ppx:addDataInfo.xmlns:xsi        ( ppx:addDataInfo + 4 ) /* XoString */
#define ppx:addDataInfo.ppx:info         ( ppx:addDataInfo + 5 ) /* ppx:AnonInfo */


/*
 *	 objet        : ppx:AnonFBD
 *	 nb attr      : 0
 */
#define ppx:AnonFBD.name$                ( ppx:AnonFBD + 1 ) /* XoString */
#define ppx:AnonFBD.xmlns                ( ppx:AnonFBD + 2 ) /* XoString */
#define ppx:AnonFBD.xmlns:xsd            ( ppx:AnonFBD + 3 ) /* XoString */
#define ppx:AnonFBD.xmlns:xsi            ( ppx:AnonFBD + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonLD
 *	 nb attr      : 0
 */
#define ppx:AnonLD.name$                 ( ppx:AnonLD + 1 ) /* XoString */
#define ppx:AnonLD.xmlns                 ( ppx:AnonLD + 2 ) /* XoString */
#define ppx:AnonLD.xmlns:xsd             ( ppx:AnonLD + 3 ) /* XoString */
#define ppx:AnonLD.xmlns:xsi             ( ppx:AnonLD + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonSFC
 *	 nb attr      : 0
 */
#define ppx:AnonSFC.name$                ( ppx:AnonSFC + 1 ) /* XoString */
#define ppx:AnonSFC.xmlns                ( ppx:AnonSFC + 2 ) /* XoString */
#define ppx:AnonSFC.xmlns:xsd            ( ppx:AnonSFC + 3 ) /* XoString */
#define ppx:AnonSFC.xmlns:xsi            ( ppx:AnonSFC + 4 ) /* XoString */


/*
 *	 objet        : ppx:body
 *	 nb attr      : 9
 */
#define ppx:body.name$                   ( ppx:body + 1 ) /* XoString */
#define ppx:body.xmlns                   ( ppx:body + 2 ) /* XoString */
#define ppx:body.xmlns:xsd               ( ppx:body + 3 ) /* XoString */
#define ppx:body.xmlns:xsi               ( ppx:body + 4 ) /* XoString */
#define ppx:body.WorksheetName           ( ppx:body + 5 ) /* XoString */
#define ppx:body.globalId                ( ppx:body + 6 ) /* XoString */
#define ppx:body.ppx:IL                  ( ppx:body + 7 ) /* ppx:formattedText */
#define ppx:body.ppx:ST                  ( ppx:body + 8 ) /* ppx:formattedText */
#define ppx:body.ppx:FBD                 ( ppx:body + 9 ) /* ppx:AnonFBD */
#define ppx:body.ppx:LD                  ( ppx:body + 10 ) /* ppx:AnonLD */
#define ppx:body.ppx:SFC                 ( ppx:body + 11 ) /* ppx:AnonSFC */
#define ppx:body.ppx:addData             ( ppx:body + 12 ) /* ppx:addData */
#define ppx:body.ppx:documentation       ( ppx:body + 13 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:dataType
 *	 nb attr      : 0
 */
#define ppx:dataType.name$               ( ppx:dataType + 1 ) /* XoString */
#define ppx:dataType.xmlns               ( ppx:dataType + 2 ) /* XoString */
#define ppx:dataType.xmlns:xsd           ( ppx:dataType + 3 ) /* XoString */
#define ppx:dataType.xmlns:xsi           ( ppx:dataType + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonSimpleValue
 *	 nb attr      : 0
 */
#define ppx:AnonSimpleValue.name$        ( ppx:AnonSimpleValue + 1 ) /* XoString */
#define ppx:AnonSimpleValue.xmlns        ( ppx:AnonSimpleValue + 2 ) /* XoString */
#define ppx:AnonSimpleValue.xmlns:xsd    ( ppx:AnonSimpleValue + 3 ) /* XoString */
#define ppx:AnonSimpleValue.xmlns:xsi    ( ppx:AnonSimpleValue + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonValue
 *	 nb attr      : 0
 */
#define ppx:AnonValue.name$              ( ppx:AnonValue + 1 ) /* XoString */
#define ppx:AnonValue.xmlns              ( ppx:AnonValue + 2 ) /* XoString */
#define ppx:AnonValue.xmlns:xsd          ( ppx:AnonValue + 3 ) /* XoString */
#define ppx:AnonValue.xmlns:xsi          ( ppx:AnonValue + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonArrayValue
 *	 nb attr      : 1
 */
#define ppx:AnonArrayValue.name$         ( ppx:AnonArrayValue + 1 ) /* XoString */
#define ppx:AnonArrayValue.xmlns         ( ppx:AnonArrayValue + 2 ) /* XoString */
#define ppx:AnonArrayValue.xmlns:xsd     ( ppx:AnonArrayValue + 3 ) /* XoString */
#define ppx:AnonArrayValue.xmlns:xsi     ( ppx:AnonArrayValue + 4 ) /* XoString */
#define ppx:AnonArrayValue.ppx:value     ( ppx:AnonArrayValue + 5 ) /* ppx:AnonValue */


/*
 *	 objet        : ppx:AnonStructValue
 *	 nb attr      : 1
 */
#define ppx:AnonStructValue.name$        ( ppx:AnonStructValue + 1 ) /* XoString */
#define ppx:AnonStructValue.xmlns        ( ppx:AnonStructValue + 2 ) /* XoString */
#define ppx:AnonStructValue.xmlns:xsd    ( ppx:AnonStructValue + 3 ) /* XoString */
#define ppx:AnonStructValue.xmlns:xsi    ( ppx:AnonStructValue + 4 ) /* XoString */
#define ppx:AnonStructValue.ppx:value    ( ppx:AnonStructValue + 5 ) /* ppx:AnonValue */


/*
 *	 objet        : ppx:value
 *	 nb attr      : 3
 */
#define ppx:value.name$                  ( ppx:value + 1 ) /* XoString */
#define ppx:value.xmlns                  ( ppx:value + 2 ) /* XoString */
#define ppx:value.xmlns:xsd              ( ppx:value + 3 ) /* XoString */
#define ppx:value.xmlns:xsi              ( ppx:value + 4 ) /* XoString */
#define ppx:value.ppx:simpleValue        ( ppx:value + 5 ) /* ppx:AnonSimpleValue */
#define ppx:value.ppx:arrayValue         ( ppx:value + 6 ) /* ppx:AnonArrayValue */
#define ppx:value.ppx:structValue        ( ppx:value + 7 ) /* ppx:AnonStructValue */


/*
 *	 objet        : ppx:AnonVariable
 *	 nb attr      : 7
 */
#define ppx:AnonVariable.name$           ( ppx:AnonVariable + 1 ) /* XoString */
#define ppx:AnonVariable.xmlns           ( ppx:AnonVariable + 2 ) /* XoString */
#define ppx:AnonVariable.xmlns:xsd       ( ppx:AnonVariable + 3 ) /* XoString */
#define ppx:AnonVariable.xmlns:xsi       ( ppx:AnonVariable + 4 ) /* XoString */
#define ppx:AnonVariable.name            ( ppx:AnonVariable + 5 ) /* XoString */
#define ppx:AnonVariable.address         ( ppx:AnonVariable + 6 ) /* XoString */
#define ppx:AnonVariable.globalId        ( ppx:AnonVariable + 7 ) /* XoString */
#define ppx:AnonVariable.ppx:type        ( ppx:AnonVariable + 8 ) /* ppx:dataType */
#define ppx:AnonVariable.ppx:initialValu ( ppx:AnonVariable + 9 ) /* ppx:value */
#define ppx:AnonVariable.ppx:addData     ( ppx:AnonVariable + 10 ) /* ppx:addData */
#define ppx:AnonVariable.ppx:documentati ( ppx:AnonVariable + 11 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:varListPlain
 *	 nb attr      : 3
 */
#define ppx:varListPlain.name$           ( ppx:varListPlain + 1 ) /* XoString */
#define ppx:varListPlain.xmlns           ( ppx:varListPlain + 2 ) /* XoString */
#define ppx:varListPlain.xmlns:xsd       ( ppx:varListPlain + 3 ) /* XoString */
#define ppx:varListPlain.xmlns:xsi       ( ppx:varListPlain + 4 ) /* XoString */
#define ppx:varListPlain.ppx:variable    ( ppx:varListPlain + 5 ) /* ppx:AnonVariable */
#define ppx:varListPlain.ppx:addData     ( ppx:varListPlain + 6 ) /* ppx:addData */
#define ppx:varListPlain.ppx:documentati ( ppx:varListPlain + 7 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:varList
 *	 nb attr      : 0
 */
#define ppx:varList.name$                ( ppx:varList + 1 ) /* XoString */
#define ppx:varList.xmlns                ( ppx:varList + 2 ) /* XoString */
#define ppx:varList.xmlns:xsd            ( ppx:varList + 3 ) /* XoString */
#define ppx:varList.xmlns:xsi            ( ppx:varList + 4 ) /* XoString */


/*
 *	 objet        : ppx:rangeUnsigned
 *	 nb attr      : 0
 */
#define ppx:rangeUnsigned.name$          ( ppx:rangeUnsigned + 1 ) /* XoString */
#define ppx:rangeUnsigned.xmlns          ( ppx:rangeUnsigned + 2 ) /* XoString */
#define ppx:rangeUnsigned.xmlns:xsd      ( ppx:rangeUnsigned + 3 ) /* XoString */
#define ppx:rangeUnsigned.xmlns:xsi      ( ppx:rangeUnsigned + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonAccessVariable
 *	 nb attr      : 6
 */
#define ppx:AnonAccessVariable.name$     ( ppx:AnonAccessVariable + 1 ) /* XoString */
#define ppx:AnonAccessVariable.xmlns     ( ppx:AnonAccessVariable + 2 ) /* XoString */
#define ppx:AnonAccessVariable.xmlns:xsd ( ppx:AnonAccessVariable + 3 ) /* XoString */
#define ppx:AnonAccessVariable.xmlns:xsi ( ppx:AnonAccessVariable + 4 ) /* XoString */
#define ppx:AnonAccessVariable.XXXalias  ( ppx:AnonAccessVariable + 5 ) /* XoString */
#define ppx:AnonAccessVariable.instanceP ( ppx:AnonAccessVariable + 6 ) /* XoString */
#define ppx:AnonAccessVariable.direction ( ppx:AnonAccessVariable + 7 ) /* XoString */
#define ppx:AnonAccessVariable.ppx:type  ( ppx:AnonAccessVariable + 8 ) /* ppx:dataType */
#define ppx:AnonAccessVariable.ppx:addDa ( ppx:AnonAccessVariable + 9 ) /* ppx:addData */
#define ppx:AnonAccessVariable.ppx:docum ( ppx:AnonAccessVariable + 10 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:varListAccess
 *	 nb attr      : 3
 */
#define ppx:varListAccess.name$          ( ppx:varListAccess + 1 ) /* XoString */
#define ppx:varListAccess.xmlns          ( ppx:varListAccess + 2 ) /* XoString */
#define ppx:varListAccess.xmlns:xsd      ( ppx:varListAccess + 3 ) /* XoString */
#define ppx:varListAccess.xmlns:xsi      ( ppx:varListAccess + 4 ) /* XoString */
#define ppx:varListAccess.ppx:accessVari ( ppx:varListAccess + 5 ) /* ppx:AnonAccessVariable */
#define ppx:varListAccess.ppx:addData    ( ppx:varListAccess + 6 ) /* ppx:addData */
#define ppx:varListAccess.ppx:documentat ( ppx:varListAccess + 7 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:AnonConfigVariable
 *	 nb attr      : 6
 */
#define ppx:AnonConfigVariable.name$     ( ppx:AnonConfigVariable + 1 ) /* XoString */
#define ppx:AnonConfigVariable.xmlns     ( ppx:AnonConfigVariable + 2 ) /* XoString */
#define ppx:AnonConfigVariable.xmlns:xsd ( ppx:AnonConfigVariable + 3 ) /* XoString */
#define ppx:AnonConfigVariable.xmlns:xsi ( ppx:AnonConfigVariable + 4 ) /* XoString */
#define ppx:AnonConfigVariable.instanceP ( ppx:AnonConfigVariable + 5 ) /* XoString */
#define ppx:AnonConfigVariable.address   ( ppx:AnonConfigVariable + 6 ) /* XoString */
#define ppx:AnonConfigVariable.ppx:type  ( ppx:AnonConfigVariable + 7 ) /* ppx:dataType */
#define ppx:AnonConfigVariable.ppx:initi ( ppx:AnonConfigVariable + 8 ) /* ppx:value */
#define ppx:AnonConfigVariable.ppx:addDa ( ppx:AnonConfigVariable + 9 ) /* ppx:addData */
#define ppx:AnonConfigVariable.ppx:docum ( ppx:AnonConfigVariable + 10 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:varListConfig
 *	 nb attr      : 3
 */
#define ppx:varListConfig.name$          ( ppx:varListConfig + 1 ) /* XoString */
#define ppx:varListConfig.xmlns          ( ppx:varListConfig + 2 ) /* XoString */
#define ppx:varListConfig.xmlns:xsd      ( ppx:varListConfig + 3 ) /* XoString */
#define ppx:varListConfig.xmlns:xsi      ( ppx:varListConfig + 4 ) /* XoString */
#define ppx:varListConfig.ppx:configVari ( ppx:varListConfig + 5 ) /* ppx:AnonConfigVariable */
#define ppx:varListConfig.ppx:addData    ( ppx:varListConfig + 6 ) /* ppx:addData */
#define ppx:varListConfig.ppx:documentat ( ppx:varListConfig + 7 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:AnonFileHeader
 *	 nb attr      : 0
 */
#define ppx:AnonFileHeader.name$         ( ppx:AnonFileHeader + 1 ) /* XoString */
#define ppx:AnonFileHeader.xmlns         ( ppx:AnonFileHeader + 2 ) /* XoString */
#define ppx:AnonFileHeader.xmlns:xsd     ( ppx:AnonFileHeader + 3 ) /* XoString */
#define ppx:AnonFileHeader.xmlns:xsi     ( ppx:AnonFileHeader + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonPageSize
 *	 nb attr      : 0
 */
#define ppx:AnonPageSize.name$           ( ppx:AnonPageSize + 1 ) /* XoString */
#define ppx:AnonPageSize.xmlns           ( ppx:AnonPageSize + 2 ) /* XoString */
#define ppx:AnonPageSize.xmlns:xsd       ( ppx:AnonPageSize + 3 ) /* XoString */
#define ppx:AnonPageSize.xmlns:xsi       ( ppx:AnonPageSize + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonScaling
 *	 nb attr      : 0
 */
#define ppx:AnonScaling.name$            ( ppx:AnonScaling + 1 ) /* XoString */
#define ppx:AnonScaling.xmlns            ( ppx:AnonScaling + 2 ) /* XoString */
#define ppx:AnonScaling.xmlns:xsd        ( ppx:AnonScaling + 3 ) /* XoString */
#define ppx:AnonScaling.xmlns:xsi        ( ppx:AnonScaling + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonFbd
 *	 nb attr      : 1
 */
#define ppx:AnonFbd.name$                ( ppx:AnonFbd + 1 ) /* XoString */
#define ppx:AnonFbd.xmlns                ( ppx:AnonFbd + 2 ) /* XoString */
#define ppx:AnonFbd.xmlns:xsd            ( ppx:AnonFbd + 3 ) /* XoString */
#define ppx:AnonFbd.xmlns:xsi            ( ppx:AnonFbd + 4 ) /* XoString */
#define ppx:AnonFbd.ppx:scaling          ( ppx:AnonFbd + 5 ) /* ppx:AnonScaling */


/*
 *	 objet        : ppx:AnonLd
 *	 nb attr      : 1
 */
#define ppx:AnonLd.name$                 ( ppx:AnonLd + 1 ) /* XoString */
#define ppx:AnonLd.xmlns                 ( ppx:AnonLd + 2 ) /* XoString */
#define ppx:AnonLd.xmlns:xsd             ( ppx:AnonLd + 3 ) /* XoString */
#define ppx:AnonLd.xmlns:xsi             ( ppx:AnonLd + 4 ) /* XoString */
#define ppx:AnonLd.ppx:scaling           ( ppx:AnonLd + 5 ) /* ppx:AnonScaling */


/*
 *	 objet        : ppx:AnonSfc
 *	 nb attr      : 1
 */
#define ppx:AnonSfc.name$                ( ppx:AnonSfc + 1 ) /* XoString */
#define ppx:AnonSfc.xmlns                ( ppx:AnonSfc + 2 ) /* XoString */
#define ppx:AnonSfc.xmlns:xsd            ( ppx:AnonSfc + 3 ) /* XoString */
#define ppx:AnonSfc.xmlns:xsi            ( ppx:AnonSfc + 4 ) /* XoString */
#define ppx:AnonSfc.ppx:scaling          ( ppx:AnonSfc + 5 ) /* ppx:AnonScaling */


/*
 *	 objet        : ppx:AnonCoordinateInfo
 *	 nb attr      : 4
 */
#define ppx:AnonCoordinateInfo.name$     ( ppx:AnonCoordinateInfo + 1 ) /* XoString */
#define ppx:AnonCoordinateInfo.xmlns     ( ppx:AnonCoordinateInfo + 2 ) /* XoString */
#define ppx:AnonCoordinateInfo.xmlns:xsd ( ppx:AnonCoordinateInfo + 3 ) /* XoString */
#define ppx:AnonCoordinateInfo.xmlns:xsi ( ppx:AnonCoordinateInfo + 4 ) /* XoString */
#define ppx:AnonCoordinateInfo.ppx:pageS ( ppx:AnonCoordinateInfo + 5 ) /* ppx:AnonPageSize */
#define ppx:AnonCoordinateInfo.ppx:fbd   ( ppx:AnonCoordinateInfo + 6 ) /* ppx:AnonFbd */
#define ppx:AnonCoordinateInfo.ppx:ld    ( ppx:AnonCoordinateInfo + 7 ) /* ppx:AnonLd */
#define ppx:AnonCoordinateInfo.ppx:sfc   ( ppx:AnonCoordinateInfo + 8 ) /* ppx:AnonSfc */


/*
 *	 objet        : ppx:AnonContentHeader
 *	 nb attr      : 10
 */
#define ppx:AnonContentHeader.name$      ( ppx:AnonContentHeader + 1 ) /* XoString */
#define ppx:AnonContentHeader.xmlns      ( ppx:AnonContentHeader + 2 ) /* XoString */
#define ppx:AnonContentHeader.xmlns:xsd  ( ppx:AnonContentHeader + 3 ) /* XoString */
#define ppx:AnonContentHeader.xmlns:xsi  ( ppx:AnonContentHeader + 4 ) /* XoString */
#define ppx:AnonContentHeader.name       ( ppx:AnonContentHeader + 5 ) /* XoString */
#define ppx:AnonContentHeader.version    ( ppx:AnonContentHeader + 6 ) /* XoString */
#define ppx:AnonContentHeader.modificati ( ppx:AnonContentHeader + 7 ) /* XoString */
#define ppx:AnonContentHeader.organizati ( ppx:AnonContentHeader + 8 ) /* XoString */
#define ppx:AnonContentHeader.author     ( ppx:AnonContentHeader + 9 ) /* XoString */
#define ppx:AnonContentHeader.language   ( ppx:AnonContentHeader + 10 ) /* XoString */
#define ppx:AnonContentHeader.ppx:Commen ( ppx:AnonContentHeader + 11 ) /* XoString */
#define ppx:AnonContentHeader.ppx:coordi ( ppx:AnonContentHeader + 12 ) /* ppx:AnonCoordinateInfo */
#define ppx:AnonContentHeader.ppx:addDat ( ppx:AnonContentHeader + 13 ) /* ppx:addDataInfo */
#define ppx:AnonContentHeader.ppx:addDat ( ppx:AnonContentHeader + 14 ) /* ppx:addData */


/*
 *	 objet        : ppx:AnonDataType
 *	 nb attr      : 5
 */
#define ppx:AnonDataType.name$           ( ppx:AnonDataType + 1 ) /* XoString */
#define ppx:AnonDataType.xmlns           ( ppx:AnonDataType + 2 ) /* XoString */
#define ppx:AnonDataType.xmlns:xsd       ( ppx:AnonDataType + 3 ) /* XoString */
#define ppx:AnonDataType.xmlns:xsi       ( ppx:AnonDataType + 4 ) /* XoString */
#define ppx:AnonDataType.name            ( ppx:AnonDataType + 5 ) /* XoString */
#define ppx:AnonDataType.ppx:baseType    ( ppx:AnonDataType + 6 ) /* ppx:dataType */
#define ppx:AnonDataType.ppx:initialValu ( ppx:AnonDataType + 7 ) /* ppx:value */
#define ppx:AnonDataType.ppx:addData     ( ppx:AnonDataType + 8 ) /* ppx:addData */
#define ppx:AnonDataType.ppx:documentati ( ppx:AnonDataType + 9 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:AnonDataTypes
 *	 nb attr      : 1
 */
#define ppx:AnonDataTypes.name$          ( ppx:AnonDataTypes + 1 ) /* XoString */
#define ppx:AnonDataTypes.xmlns          ( ppx:AnonDataTypes + 2 ) /* XoString */
#define ppx:AnonDataTypes.xmlns:xsd      ( ppx:AnonDataTypes + 3 ) /* XoString */
#define ppx:AnonDataTypes.xmlns:xsi      ( ppx:AnonDataTypes + 4 ) /* XoString */
#define ppx:AnonDataTypes.ppx:dataType   ( ppx:AnonDataTypes + 5 ) /* ppx:AnonDataType */


/*
 *	 objet        : ppx:AnonLocalVars
 *	 nb attr      : 0
 */
#define ppx:AnonLocalVars.name$          ( ppx:AnonLocalVars + 1 ) /* XoString */
#define ppx:AnonLocalVars.xmlns          ( ppx:AnonLocalVars + 2 ) /* XoString */
#define ppx:AnonLocalVars.xmlns:xsd      ( ppx:AnonLocalVars + 3 ) /* XoString */
#define ppx:AnonLocalVars.xmlns:xsi      ( ppx:AnonLocalVars + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonTempVars
 *	 nb attr      : 0
 */
#define ppx:AnonTempVars.name$           ( ppx:AnonTempVars + 1 ) /* XoString */
#define ppx:AnonTempVars.xmlns           ( ppx:AnonTempVars + 2 ) /* XoString */
#define ppx:AnonTempVars.xmlns:xsd       ( ppx:AnonTempVars + 3 ) /* XoString */
#define ppx:AnonTempVars.xmlns:xsi       ( ppx:AnonTempVars + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonInputVars
 *	 nb attr      : 0
 */
#define ppx:AnonInputVars.name$          ( ppx:AnonInputVars + 1 ) /* XoString */
#define ppx:AnonInputVars.xmlns          ( ppx:AnonInputVars + 2 ) /* XoString */
#define ppx:AnonInputVars.xmlns:xsd      ( ppx:AnonInputVars + 3 ) /* XoString */
#define ppx:AnonInputVars.xmlns:xsi      ( ppx:AnonInputVars + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonOutputVars
 *	 nb attr      : 0
 */
#define ppx:AnonOutputVars.name$         ( ppx:AnonOutputVars + 1 ) /* XoString */
#define ppx:AnonOutputVars.xmlns         ( ppx:AnonOutputVars + 2 ) /* XoString */
#define ppx:AnonOutputVars.xmlns:xsd     ( ppx:AnonOutputVars + 3 ) /* XoString */
#define ppx:AnonOutputVars.xmlns:xsi     ( ppx:AnonOutputVars + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonInOutVars
 *	 nb attr      : 0
 */
#define ppx:AnonInOutVars.name$          ( ppx:AnonInOutVars + 1 ) /* XoString */
#define ppx:AnonInOutVars.xmlns          ( ppx:AnonInOutVars + 2 ) /* XoString */
#define ppx:AnonInOutVars.xmlns:xsd      ( ppx:AnonInOutVars + 3 ) /* XoString */
#define ppx:AnonInOutVars.xmlns:xsi      ( ppx:AnonInOutVars + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonExternalVars
 *	 nb attr      : 0
 */
#define ppx:AnonExternalVars.name$       ( ppx:AnonExternalVars + 1 ) /* XoString */
#define ppx:AnonExternalVars.xmlns       ( ppx:AnonExternalVars + 2 ) /* XoString */
#define ppx:AnonExternalVars.xmlns:xsd   ( ppx:AnonExternalVars + 3 ) /* XoString */
#define ppx:AnonExternalVars.xmlns:xsi   ( ppx:AnonExternalVars + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonGlobalVars
 *	 nb attr      : 0
 */
#define ppx:AnonGlobalVars.name$         ( ppx:AnonGlobalVars + 1 ) /* XoString */
#define ppx:AnonGlobalVars.xmlns         ( ppx:AnonGlobalVars + 2 ) /* XoString */
#define ppx:AnonGlobalVars.xmlns:xsd     ( ppx:AnonGlobalVars + 3 ) /* XoString */
#define ppx:AnonGlobalVars.xmlns:xsi     ( ppx:AnonGlobalVars + 4 ) /* XoString */


/*
 *	 objet        : ppx:AnonInterface
 *	 nb attr      : 11
 */
#define ppx:AnonInterface.name$          ( ppx:AnonInterface + 1 ) /* XoString */
#define ppx:AnonInterface.xmlns          ( ppx:AnonInterface + 2 ) /* XoString */
#define ppx:AnonInterface.xmlns:xsd      ( ppx:AnonInterface + 3 ) /* XoString */
#define ppx:AnonInterface.xmlns:xsi      ( ppx:AnonInterface + 4 ) /* XoString */
#define ppx:AnonInterface.ppx:returnType ( ppx:AnonInterface + 5 ) /* ppx:dataType */
#define ppx:AnonInterface.ppx:localVars  ( ppx:AnonInterface + 6 ) /* ppx:AnonLocalVars */
#define ppx:AnonInterface.ppx:tempVars   ( ppx:AnonInterface + 7 ) /* ppx:AnonTempVars */
#define ppx:AnonInterface.ppx:inputVars  ( ppx:AnonInterface + 8 ) /* ppx:AnonInputVars */
#define ppx:AnonInterface.ppx:outputVars ( ppx:AnonInterface + 9 ) /* ppx:AnonOutputVars */
#define ppx:AnonInterface.ppx:inOutVars  ( ppx:AnonInterface + 10 ) /* ppx:AnonInOutVars */
#define ppx:AnonInterface.ppx:externalVa ( ppx:AnonInterface + 11 ) /* ppx:AnonExternalVars */
#define ppx:AnonInterface.ppx:globalVars ( ppx:AnonInterface + 12 ) /* ppx:AnonGlobalVars */
#define ppx:AnonInterface.ppx:accessVars ( ppx:AnonInterface + 13 ) /* ppx:varList */
#define ppx:AnonInterface.ppx:addData    ( ppx:AnonInterface + 14 ) /* ppx:addData */
#define ppx:AnonInterface.ppx:documentat ( ppx:AnonInterface + 15 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:AnonAction
 *	 nb attr      : 5
 */
#define ppx:AnonAction.name$             ( ppx:AnonAction + 1 ) /* XoString */
#define ppx:AnonAction.xmlns             ( ppx:AnonAction + 2 ) /* XoString */
#define ppx:AnonAction.xmlns:xsd         ( ppx:AnonAction + 3 ) /* XoString */
#define ppx:AnonAction.xmlns:xsi         ( ppx:AnonAction + 4 ) /* XoString */
#define ppx:AnonAction.name              ( ppx:AnonAction + 5 ) /* XoString */
#define ppx:AnonAction.globalId          ( ppx:AnonAction + 6 ) /* XoString */
#define ppx:AnonAction.ppx:body          ( ppx:AnonAction + 7 ) /* ppx:body */
#define ppx:AnonAction.ppx:addData       ( ppx:AnonAction + 8 ) /* ppx:addData */
#define ppx:AnonAction.ppx:documentation ( ppx:AnonAction + 9 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:AnonActions
 *	 nb attr      : 1
 */
#define ppx:AnonActions.name$            ( ppx:AnonActions + 1 ) /* XoString */
#define ppx:AnonActions.xmlns            ( ppx:AnonActions + 2 ) /* XoString */
#define ppx:AnonActions.xmlns:xsd        ( ppx:AnonActions + 3 ) /* XoString */
#define ppx:AnonActions.xmlns:xsi        ( ppx:AnonActions + 4 ) /* XoString */
#define ppx:AnonActions.ppx:action       ( ppx:AnonActions + 5 ) /* ppx:AnonAction */


/*
 *	 objet        : ppx:AnonTransition
 *	 nb attr      : 5
 */
#define ppx:AnonTransition.name$         ( ppx:AnonTransition + 1 ) /* XoString */
#define ppx:AnonTransition.xmlns         ( ppx:AnonTransition + 2 ) /* XoString */
#define ppx:AnonTransition.xmlns:xsd     ( ppx:AnonTransition + 3 ) /* XoString */
#define ppx:AnonTransition.xmlns:xsi     ( ppx:AnonTransition + 4 ) /* XoString */
#define ppx:AnonTransition.name          ( ppx:AnonTransition + 5 ) /* XoString */
#define ppx:AnonTransition.globalId      ( ppx:AnonTransition + 6 ) /* XoString */
#define ppx:AnonTransition.ppx:body      ( ppx:AnonTransition + 7 ) /* ppx:body */
#define ppx:AnonTransition.ppx:addData   ( ppx:AnonTransition + 8 ) /* ppx:addData */
#define ppx:AnonTransition.ppx:documenta ( ppx:AnonTransition + 9 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:AnonTransitions
 *	 nb attr      : 1
 */
#define ppx:AnonTransitions.name$        ( ppx:AnonTransitions + 1 ) /* XoString */
#define ppx:AnonTransitions.xmlns        ( ppx:AnonTransitions + 2 ) /* XoString */
#define ppx:AnonTransitions.xmlns:xsd    ( ppx:AnonTransitions + 3 ) /* XoString */
#define ppx:AnonTransitions.xmlns:xsi    ( ppx:AnonTransitions + 4 ) /* XoString */
#define ppx:AnonTransitions.ppx:transiti ( ppx:AnonTransitions + 5 ) /* ppx:AnonTransition */


/*
 *	 objet        : ppx:AnonPou
 *	 nb attr      : 9
 */
#define ppx:AnonPou.name$                ( ppx:AnonPou + 1 ) /* XoString */
#define ppx:AnonPou.xmlns                ( ppx:AnonPou + 2 ) /* XoString */
#define ppx:AnonPou.xmlns:xsd            ( ppx:AnonPou + 3 ) /* XoString */
#define ppx:AnonPou.xmlns:xsi            ( ppx:AnonPou + 4 ) /* XoString */
#define ppx:AnonPou.name                 ( ppx:AnonPou + 5 ) /* XoString */
#define ppx:AnonPou.pouType              ( ppx:AnonPou + 6 ) /* XoString */
#define ppx:AnonPou.globalId             ( ppx:AnonPou + 7 ) /* XoString */
#define ppx:AnonPou.ppx:interface        ( ppx:AnonPou + 8 ) /* ppx:AnonInterface */
#define ppx:AnonPou.ppx:actions          ( ppx:AnonPou + 9 ) /* ppx:AnonActions */
#define ppx:AnonPou.ppx:transitions      ( ppx:AnonPou + 10 ) /* ppx:AnonTransitions */
#define ppx:AnonPou.ppx:body             ( ppx:AnonPou + 11 ) /* ppx:body */
#define ppx:AnonPou.ppx:addData          ( ppx:AnonPou + 12 ) /* ppx:addData */
#define ppx:AnonPou.ppx:documentation    ( ppx:AnonPou + 13 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:AnonPous
 *	 nb attr      : 1
 */
#define ppx:AnonPous.name$               ( ppx:AnonPous + 1 ) /* XoString */
#define ppx:AnonPous.xmlns               ( ppx:AnonPous + 2 ) /* XoString */
#define ppx:AnonPous.xmlns:xsd           ( ppx:AnonPous + 3 ) /* XoString */
#define ppx:AnonPous.xmlns:xsi           ( ppx:AnonPous + 4 ) /* XoString */
#define ppx:AnonPous.ppx:pou             ( ppx:AnonPous + 5 ) /* ppx:AnonPou */


/*
 *	 objet        : ppx:AnonTypes
 *	 nb attr      : 2
 */
#define ppx:AnonTypes.name$              ( ppx:AnonTypes + 1 ) /* XoString */
#define ppx:AnonTypes.xmlns              ( ppx:AnonTypes + 2 ) /* XoString */
#define ppx:AnonTypes.xmlns:xsd          ( ppx:AnonTypes + 3 ) /* XoString */
#define ppx:AnonTypes.xmlns:xsi          ( ppx:AnonTypes + 4 ) /* XoString */
#define ppx:AnonTypes.ppx:dataTypes      ( ppx:AnonTypes + 5 ) /* ppx:AnonDataTypes */
#define ppx:AnonTypes.ppx:pous           ( ppx:AnonTypes + 6 ) /* ppx:AnonPous */


/*
 *	 objet        : ppx:AnonTask
 *	 nb attr      : 8
 */
#define ppx:AnonTask.name$               ( ppx:AnonTask + 1 ) /* XoString */
#define ppx:AnonTask.xmlns               ( ppx:AnonTask + 2 ) /* XoString */
#define ppx:AnonTask.xmlns:xsd           ( ppx:AnonTask + 3 ) /* XoString */
#define ppx:AnonTask.xmlns:xsi           ( ppx:AnonTask + 4 ) /* XoString */
#define ppx:AnonTask.name                ( ppx:AnonTask + 5 ) /* XoString */
#define ppx:AnonTask.single              ( ppx:AnonTask + 6 ) /* XoString */
#define ppx:AnonTask.interval            ( ppx:AnonTask + 7 ) /* XoString */
#define ppx:AnonTask.priority            ( ppx:AnonTask + 8 ) /* XoString */
#define ppx:AnonTask.globalId            ( ppx:AnonTask + 9 ) /* XoString */
#define ppx:AnonTask.ppx:pouInstance     ( ppx:AnonTask + 10 ) /* ppx:pouInstance */
#define ppx:AnonTask.ppx:addData         ( ppx:AnonTask + 11 ) /* ppx:addData */
#define ppx:AnonTask.ppx:documentation   ( ppx:AnonTask + 12 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:AnonResource
 *	 nb attr      : 7
 */
#define ppx:AnonResource.name$           ( ppx:AnonResource + 1 ) /* XoString */
#define ppx:AnonResource.xmlns           ( ppx:AnonResource + 2 ) /* XoString */
#define ppx:AnonResource.xmlns:xsd       ( ppx:AnonResource + 3 ) /* XoString */
#define ppx:AnonResource.xmlns:xsi       ( ppx:AnonResource + 4 ) /* XoString */
#define ppx:AnonResource.name            ( ppx:AnonResource + 5 ) /* XoString */
#define ppx:AnonResource.globalId        ( ppx:AnonResource + 6 ) /* XoString */
#define ppx:AnonResource.ppx:task        ( ppx:AnonResource + 7 ) /* ppx:AnonTask */
#define ppx:AnonResource.ppx:globalVars  ( ppx:AnonResource + 8 ) /* ppx:varList */
#define ppx:AnonResource.ppx:pouInstance ( ppx:AnonResource + 9 ) /* ppx:pouInstance */
#define ppx:AnonResource.ppx:addData     ( ppx:AnonResource + 10 ) /* ppx:addData */
#define ppx:AnonResource.ppx:documentati ( ppx:AnonResource + 11 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:AnonConfiguration
 *	 nb attr      : 8
 */
#define ppx:AnonConfiguration.name$      ( ppx:AnonConfiguration + 1 ) /* XoString */
#define ppx:AnonConfiguration.xmlns      ( ppx:AnonConfiguration + 2 ) /* XoString */
#define ppx:AnonConfiguration.xmlns:xsd  ( ppx:AnonConfiguration + 3 ) /* XoString */
#define ppx:AnonConfiguration.xmlns:xsi  ( ppx:AnonConfiguration + 4 ) /* XoString */
#define ppx:AnonConfiguration.name       ( ppx:AnonConfiguration + 5 ) /* XoString */
#define ppx:AnonConfiguration.globalId   ( ppx:AnonConfiguration + 6 ) /* XoString */
#define ppx:AnonConfiguration.ppx:resour ( ppx:AnonConfiguration + 7 ) /* ppx:AnonResource */
#define ppx:AnonConfiguration.ppx:global ( ppx:AnonConfiguration + 8 ) /* ppx:varList */
#define ppx:AnonConfiguration.ppx:access ( ppx:AnonConfiguration + 9 ) /* ppx:varListAccess */
#define ppx:AnonConfiguration.ppx:config ( ppx:AnonConfiguration + 10 ) /* ppx:varListConfig */
#define ppx:AnonConfiguration.ppx:addDat ( ppx:AnonConfiguration + 11 ) /* ppx:addData */
#define ppx:AnonConfiguration.ppx:docume ( ppx:AnonConfiguration + 12 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:AnonConfigurations
 *	 nb attr      : 1
 */
#define ppx:AnonConfigurations.name$     ( ppx:AnonConfigurations + 1 ) /* XoString */
#define ppx:AnonConfigurations.xmlns     ( ppx:AnonConfigurations + 2 ) /* XoString */
#define ppx:AnonConfigurations.xmlns:xsd ( ppx:AnonConfigurations + 3 ) /* XoString */
#define ppx:AnonConfigurations.xmlns:xsi ( ppx:AnonConfigurations + 4 ) /* XoString */
#define ppx:AnonConfigurations.ppx:confi ( ppx:AnonConfigurations + 5 ) /* ppx:AnonConfiguration */


/*
 *	 objet        : ppx:AnonInstances
 *	 nb attr      : 1
 */
#define ppx:AnonInstances.name$          ( ppx:AnonInstances + 1 ) /* XoString */
#define ppx:AnonInstances.xmlns          ( ppx:AnonInstances + 2 ) /* XoString */
#define ppx:AnonInstances.xmlns:xsd      ( ppx:AnonInstances + 3 ) /* XoString */
#define ppx:AnonInstances.xmlns:xsi      ( ppx:AnonInstances + 4 ) /* XoString */
#define ppx:AnonInstances.ppx:configurat ( ppx:AnonInstances + 5 ) /* ppx:AnonConfigurations */


/*
 *	 objet        : ppx:AnonProject
 *	 nb attr      : 6
 */
#define ppx:AnonProject.name$            ( ppx:AnonProject + 1 ) /* XoString */
#define ppx:AnonProject.xmlns            ( ppx:AnonProject + 2 ) /* XoString */
#define ppx:AnonProject.xmlns:xsd        ( ppx:AnonProject + 3 ) /* XoString */
#define ppx:AnonProject.xmlns:xsi        ( ppx:AnonProject + 4 ) /* XoString */
#define ppx:AnonProject.ppx:fileHeader   ( ppx:AnonProject + 5 ) /* ppx:AnonFileHeader */
#define ppx:AnonProject.ppx:contentHeade ( ppx:AnonProject + 6 ) /* ppx:AnonContentHeader */
#define ppx:AnonProject.ppx:types        ( ppx:AnonProject + 7 ) /* ppx:AnonTypes */
#define ppx:AnonProject.ppx:instances    ( ppx:AnonProject + 8 ) /* ppx:AnonInstances */
#define ppx:AnonProject.ppx:addData      ( ppx:AnonProject + 9 ) /* ppx:addData */
#define ppx:AnonProject.ppx:documentatio ( ppx:AnonProject + 10 ) /* ppx:formattedText */


/*
 *	 objet        : ppx:project
 *	 nb attr      : 0
 */


