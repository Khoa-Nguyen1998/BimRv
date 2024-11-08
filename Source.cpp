#include "BmJsonExport.h"

#include "RxModelHierarchyTreeBase.h"
#include "RxModelTreeBaseNode.h"
#include "toString.h"
#include "Base/BmParameterSet.h"

OdString str_Object("object");
OdString str_Objects("objects");
OdString str_ObjectId("objectId");
OdString str_Name("name");
OdString str_ExternalId("externalId");
OdString str_Yes("Yes");
OdString str_No("No");
OdString str_None("None");

namespace DEMO
{
	const OdString writeTreeObjectToFile(OdBmDatabasePtr& pDb, bool bCDATree)
	{
		StringBuffer sb;
		PrettyWriter<StringBuffer> writer(sb);
		OdDbHandle handle;

		writer.StartObject();
		writer.String("data");

		writer.StartObject();
		writer.String("type");
		writer.String(str_Objects);

		writer.String(str_Objects);
		writer.StartArray();

		writer.StartObject();
		writer.String(str_ObjectId);
		writer.String("");

		writer.String(str_Name);
		writer.String("Model");

		//writer.String("externalid");
		//writer.String("doc_" + getUUID());

		writer.String(str_Objects);
		writer.StartArray();

		writer.EndArray();

		writer.EndObject();
		writer.EndArray();

		writer.EndObject();
		writer.EndObject();
		const OdString str = sb.GetString();
		return str;
	}

	const OdString writePropertyToFile(OdBmDatabasePtr& pDb, OdDbHandle handleOneObj)
	{
		StringBuffer sb;
		PrettyWriter<StringBuffer> writer(sb);

		writer.StartObject();

		writer.String("data");
		writer.StartObject();

		writer.String("type");
		writer.String("properties");

		writer.String("collection");
		writer.StartArray();

		writer.EndArray();
		writer.EndObject();
		writer.EndObject();

		const char* str = sb.GetString();
		return str;
	}
};