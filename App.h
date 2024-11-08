#pragma once
#include "BimCommon.h"
#include "BmJsonExportDef.h"
#include "StaticRxObject.h"
#include "DynamicLinker.h"
#include "ExBimHostAppServices.h"

#include "Database/BmDatabase.h"
#include "Database/BmElement.h"
#include "Common/BuiltIns/BmBuiltInParameter.h"
#include "Database/Entities/BmParamElem.h"
#include "Database/Entities/BmDBDrawing.h"
#include "Database/Entities/BmDBView.h"
#include "Database/Entities/BmViewport.h"
#include "Database/PE/BmLabelUtilsPE.h"

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

namespace DEMO
{
	const OdString writePropertyToFile(OdBmDatabasePtr& pDb, OdDbHandle handleOneObj);
	const OdString writeTreeObjectToFile(OdBmDatabasePtr& pDb, bool bCDATree = false);
};
