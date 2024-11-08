#include "BimCommon.h"
#include "StaticRxObject.h"
#include "ExBimHostAppServices.h"
#include "Database/BmDatabase.h"
#include "DynamicLinker.h"
#include "RxInit.h"
#include "RxDynamicModule.h"
#include <iostream>
#include <string>
#include "BmElementCollector.h"
#include "BmDatabase.h"
#include <Geometry/Entities/BmGElement.h>
#include <Geometry/Entities/BmGeometry.h>
#include <Geometry/Entities/BmGEdge.h>
#include <Geometry/Entities/BmGEdgeLoop.h>
#include "Geometry/Entities/BmGFace.h"
#include "Geometry/Entities/BmGNode.h"
#include "Database/BmSpecialId.h"
#include "Geometry/Entities/BmGFace.h"
#include "RxObject.h"
#include "App.h"
#include "DataOutput.h"
#include <Ed/EdCommandContext.h>
#include <fstream>
#include "Geometry/Entities/BmGFilter.h"

#include "Database/Entities/BmSerializedDummy.h"
#include "Database/BmGeometryOption.h"
#include "Geometry/Entities/BmGElement.h"
#include "Geometry/Entities/BmGeometry.h"
#include "Geometry/Entities/BmGInstance.h"
#include "Geometry/Entities/BmFace.h"
#include "Geometry/Entities/BmEdge.h"
#include "Geometry/Entities/BmGEdgeLoop.h"
#include <Ge/GeVector3d.h>
#include <Ge/GeSurface.h>

#include <Ge/GeSurface.h>
#include <Ge/GePlane.h>
#include <Ge/GeCylinder.h>
#include <Ge/GeSphere.h>
#include <Ge/GeVector3d.h>
#include <Ge/GePoint3d.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class MyServices : public ExSystemServices, public OdExBimHostAppServices
{
protected:
	ODRX_USING_HEAP_OPERATORS(ExSystemServices);
};

#pragma region json

// Function to convert Model data to JSON
json pointToJson(const DEMO::Point& point) {
	return { {"x", point.x}, {"y", point.y}, {"z", point.z} };
}

json edgeToJson(const DEMO::Edge& edge) {
	json edgeJson = json::array();
	for (const auto& point : edge.points) {
		edgeJson.push_back(pointToJson(point));
	}
	return edgeJson;
}

json edgeLoopToJson(const DEMO::EdgeLoop& edgeLoop) {
	json edgeLoopJson = json::array();
	for (const auto& edge : edgeLoop.edges) {
		edgeLoopJson.push_back(edgeToJson(edge));
	}
	return edgeLoopJson;
}

json faceToJson(const DEMO::Face& face) {
	json faceJson = json::array();
	for (const auto& edgeLoop : face.edgeLoops) {
		faceJson.push_back(edgeLoopToJson(edgeLoop));
	}
	return faceJson;
}

json geometryToJson(const DEMO::Geometry& geometry) {
	json geometryJson = json::array();
	for (const auto& face : geometry.faces) {
		geometryJson.push_back(faceToJson(face));
	}
	return geometryJson;
}

json elementToJson(const DEMO::Element& element) {
	return {
		{"id", element.id},
		{"category", element.category},
		{"geometry", geometryToJson(element.geometry)}
	};
}

json modelToJson(const DEMO::Model& model) {
	json modelJson;
	modelJson["elements"] = json::array();
	for (const auto& element : model.elements) {
		modelJson["elements"].push_back(elementToJson(element));
	}
	return modelJson;
}

void writeModelToJSONFile(DEMO::Model model, OdString  filename) {
	json modelJson = modelToJson(model);

	std::ofstream file(filename.c_str());
	file << modelJson.dump(4);  // Dump with 4-space indentation
	file.close();
}

#pragma endregion json

void printBRepGeometry(const OdBmGeometryPtr& pGeometry)
{
	OdString message;
	// Get faces
	OdBmFacePtrArray faceNodes;
	pGeometry->getFaces(faceNodes);
	message.format(L"    Number of faces is %i", faceNodes.size());

	// Use Face Iterator to iterate all faces of the object
	int faceNum = 0;
	for (OdBmFacePtrArray::const_iterator faceIt = faceNodes.begin(); faceIt != faceNodes.end(); ++faceIt)
	{
		message.format(L"      Face %i", faceNum);

		faceNum++;
		// Get first EdgeLoop of a face
		OdBmFacePtr pFace = *faceIt;

		OdBmGEdgeLoopPtr pFirstEdgeLoop = pFace->getFirstLoop();
		if (pFirstEdgeLoop.isNull()) //it is possible
			continue;

		int loopNum = 0;
		OdBmGEdgeLoopPtr pNextEdgeLoop = pFirstEdgeLoop;
		while (!pNextEdgeLoop.isNull())
		{
			message.format(L"        Loop %i", loopNum);

			// Get first edge in loop
			OdBmGEdgeBase* pNext = pNextEdgeLoop->getNext();
			int edgeNum = 0;
			while (!pNext->isLoop())
			{
				OdBmGEdge* pGEdge = dynamic_cast<OdBmGEdge*>(pNext);

				bool bForvardDir = true;
				int iEdgeForFace = 0;
				{
					OdBmFace* pFaceInt = pGEdge->getFacesItem(iEdgeForFace);
					if (pFaceInt != pFace.get())
					{
						pFaceInt = pGEdge->getFacesItem(1);
						if (pFaceInt == pFace.get())
						{
							bForvardDir = false;
							iEdgeForFace = 1;
						}
					}
				}

				if (pGEdge->isFlipped())
					bForvardDir = !bForvardDir;

				// Get start and end points of the Edge
				OdGePoint3d ptStart, ptEnd;
				pGEdge->getFirstAndLastEdgeGePnt(ptStart, false);
				pGEdge->getFirstAndLastEdgeGePnt(ptStart, false);
				pGEdge->getFirstAndLastEdgeGePnt(ptEnd, true);
				if (bForvardDir)
					message.format(L"          Edge %i  points: [%f , %f , %f], [%f , %f , %f]", edgeNum++, ptStart.x, ptStart.y, ptStart.z, ptEnd.x, ptEnd.y, ptEnd.z);
				else
					message.format(L"          Edge %i  points: [%f , %f , %f], [%f , %f , %f]", edgeNum++, ptEnd.x, ptEnd.y, ptEnd.z, ptStart.x, ptStart.y, ptStart.z);

				// Go to next edge in loop
				try
				{
					pNext = pGEdge->getNextItem(iEdgeForFace);
					OdBmGCurvePtr gCurve;
					pGEdge->getGCurve(gCurve);
				}
				catch (const OdError& err)
				{
					message.format(L"%ls", err.description().c_str());
					break;
				}
			}//while (!pNext->isLoop())
			loopNum++;
			pNextEdgeLoop = pNextEdgeLoop->getNextLoop();
		}//while (!pNextEdgeLoop.isNull())
	}//for (OdBmFacePtrArray::iterator faceIt = faceNodes.begin(); faceIt != faceNodes.end(); ++faceIt){}
}

void dumpGGroup(OdBmGNodePtrArray& nodes) {
	for (OdBmGNodePtr node : nodes)
	{
		if (node->isA() == OdBmGeometry::desc()) {
			const OdBmGeometry* pGeometry = dynamic_cast<const OdBmGeometry*>(node.get());
			printBRepGeometry(pGeometry);
		}//if (nodes[0]->isA() == OdBmGeometry::desc())
		else if ((node->isA() == OdBmGGroup::desc()) || (node->isA() == OdBmGFilter::desc())) {
			const OdBmGGroup* pGroup = dynamic_cast<const OdBmGGroup*>(node.get());
			OdBmGNodePtrArray nodes;
			pGroup->getAllSubNodes(nodes);
			dumpGGroup(nodes);
		}//if (nodes[0]->isA() == OdBmGeometry::desc())
	}//for (OdBmGNodePtrArray::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
}

DEMO::Model GetDataGeometryJson(OdBmDatabasePtr pDb, OdInt32 id) {
	DEMO::Model	model;

	try
	{
		OdDbHandle handleOneObj(id);
		if (handleOneObj.isNull() != false) {
			OdBmElementPtr pElement = pDb->getObjectId(handleOneObj).safeOpenObject();

			DEMO::Element elemJson(1, "khoa");
			model.elements.push_back(elemJson);

			//_____Get object's geometry_____//
			OdBmObjectPtr pGeom = pElement->getGeometry();
			if (pGeom->isA() == OdBmGElement::desc())
			{
				OdBmGElement* pGElem = dynamic_cast<OdBmGElement*>(pGeom.get());

				//Get nodes
				OdBmGNodePtrArray nodes;
				pGElem->getAllSubNodes(nodes);

				dumpGGroup(nodes);
			}
		}
	}
	catch (std::exception)
	{
		DEMO::Element element(1, "error");
		model.elements.push_back(element);
	}
	return model;
}

void ExportJsonFile(OdString pathOut) {
	// Create an output file stream (ofstream) to create and write to a file
	std::ofstream outFile(pathOut.c_str());

	// Check if the file was created successfully
	if (outFile.is_open()) {
		// Write some text to the file
		outFile << "Hello, World!" << std::endl;
		outFile << "This is a sample text file created using C++." << std::endl;

		// Close the file after writing
		outFile.close();

		std::cout << "File created and text written successfully!" << std::endl;
	}
	else {
		std::cerr << "Unable to create or open the file." << std::endl;
	}
}

int main()
{
	std::string pathIn, pathOut;
	std::cout << "Please enter the path to the Revit file: ";
	std::cin >> pathIn;
	std::cout << "The path to the Revit file: " + pathIn;

	pathOut = pathIn;
	size_t dotPosition = pathOut.find_last_of('.');
	// Check if the dot was found and replace the extension
	if (dotPosition != std::string::npos) {
		pathOut.replace(dotPosition, std::string::npos, ".json");
	}

	std::cout << "\nThe path to the JSON output file: " + pathOut + "\n";

	OdString inFile(pathIn.c_str());
	OdString outFile(pathOut.c_str());

	// Create a custom Services object
	OdStaticRxObject < MyServices > svcs;

	// Initialize Runtime Extension environment
	odrxInitialize(&svcs);

	try
	{
		// Load dynamic module for BimRv
		::odrxDynamicLinker()->loadModule(OdBmLoaderModuleName, false);

		// Create a BimRv database and fills it with input file content
		OdBmDatabasePtr pDb = svcs.readFile(inFile);
		OdInt32 id = 179890;

		DEMO::Model model = GetDataGeometryJson(pDb, id);

		// Export model to JSON file
		writeModelToJSONFile(model, outFile);
	}
	catch (OdError& err)
	{
		odPrintConsoleString(L"Error during copy test.rvt file: %ls\n", err.description().c_str());
	}
	//ExportJsonFile(outFile);

	// Uninitializes Runtime Extension environment
	odrxUninitialize();

	std::string stop;
	std::cin >> stop;
	return 0;
}