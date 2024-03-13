#include "stdafx.h"
#include "RenderLib.h"
#include "spline.h"
#include "BezierPatch.h"
#include "Physics.h"
#include <random>

std::vector<glm::vec3> splinePoints;
std::vector<glm::vec3> spline2Points;
std::vector<glm::vec3> spline3Points;
std::vector<glm::vec3> spline4Points;

// callback for animating on a spline
class SplineAnimationCallback : public UpdateCallback
{
	Spline* spline;
	TransformNode* trans;
	Terrain* terrain;


	float rate;
	float t = 0;

public:
	SplineAnimationCallback(TransformNode* trans, Spline* spline, float rate) : trans(trans), spline(spline), rate(rate), t(0.0f) {}
	virtual void operator()(float dt)
	{
		t += dt;
		if (t > spline->GetNumPoints())
			t -= spline->GetNumPoints();
		// work out the new transform here
		glm::vec3 position = spline->GetPoint(t);
		glm::vec3 tangent = spline->GetTangent(t);
		glm::mat4 lookAt = glm::lookAt(position, position + tangent, glm::vec3(0, 0.5, 0));
		trans->SetTransform(glm::inverse(lookAt));
	}
};

class RotatorCallback : public UpdateCallback
{
	TransformNode* trans;
	float rate;
	float t;
public:
	RotatorCallback(TransformNode *trans, float rate) : trans(trans), rate(rate), t(0.0f) {}
	virtual void operator()(float dt)
	{
		t += dt;
		trans->SetTransform(glm::rotate(glm::mat4(), t * rate, glm::vec3(0, 1, 0)));
	}
};

Node* Scene(Physics* phys)
{
	QuadGeometry quad(10);
	TexturedLit* floor = new TexturedLit(quad, "textures/cage.jpg");
	CubeGeometry cube;//for cube geometry
	SphereGeometry* drop;// for sphere geometry	
	CylinderGeometry cup;//for cylinders
	SphereGeometry sphere(20);//for the planets

	//Scene Graph
	TransformNode* rootNode = new TransformNode(glm::mat4());
	LightNode* light = new LightNode(glm::vec3(0, 100, 0), glm::vec4(1, 1, 1, 1), 0.1, 0.9, 0.4, 10, 200);

	// create a cubemap - top, bottom, front, back, right, left
	std::vector<std::string> cubeMapFiles;
	cubeMapFiles.push_back("textures/cubemaps/top.png");
	cubeMapFiles.push_back("textures/cubemaps/bottom.png");
	cubeMapFiles.push_back("textures/cubemaps/front.png");
	cubeMapFiles.push_back("textures/cubemaps/back.png");
	cubeMapFiles.push_back("textures/cubemaps/right.png");
	cubeMapFiles.push_back("textures/cubemaps/left.png");

	SOF::Cubemap* cubeMap = new SOF::Cubemap(cubeMapFiles);//use cm array
	Skybox* sb = new Skybox(cubeMap);
	GeometryNode* sbNode = new GeometryNode(sb);
	light->AddChild(sbNode);//adding the skybox as child to root note, renders
	//before the terrain as we want it to render first.

	//TERRAIN------------------
	//create a terrain for us
	Terrain* terrain = new Terrain("terrain_textures/mountains512.png", "terrain_textures/splatmap.png", "terrain_textures/grass.jpg", "terrain_textures/path.jpg",
		"terrain_textures/dirt.jpg", 50.0f, 4.0f);
	std::vector<glm::vec3> terr;
	terr.push_back(glm::vec3(15, 9, -20));
	light->AddChild(new GeometryNode(terrain));
	TexturedLit* metal = new TexturedLit(quad, "textures/marble.png");

	//lights
	LightNode* streetlight = new LightNode(glm::vec3(4,4,14), glm::vec4(1,1,1,1), 0.1f, 1.0,0.2, 10,200);
	light->AddChild(streetlight);

	//meshes
	MeshObject* rock = new MeshObject("meshes/rock1.obj");
	MeshObject* bagel = new MeshObject("meshes/bagel.obj");
	TexturedLit* bagelN = new TexturedLit(quad, "textures/wood_diffuse.jpg", "textures/wood_normal.png");
	MeshObject* man_made = new MeshObject("meshes/manmade.obj");

	//TexturedLit* nice = new TexturedLit(bagel, "textures/marble.png");

	GeometryNode* geomNode = new GeometryNode(floor);
	// scale the floor and add 
	glm::mat4 floorMat = glm::scale(glm::mat4(), glm::vec3(100, 100, 100));
	TransformNode* floorScale = new TransformNode(floorMat);
	light->AddChild(floorScale);
	floorScale->AddChild(geomNode);
	phys->AddCollider(new QuadCollider(floorMat));

	// make some walls
	glm::mat4 wallTrans = glm::mat4();
	wallTrans = glm::translate(wallTrans, glm::vec3(50.0, 0.0, 0.0));
	wallTrans = glm::rotate(wallTrans, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0));
	wallTrans = glm::scale(wallTrans, glm::vec3(10.0f, 100.0f, 100.0f));
	TransformNode *wall0 = new TransformNode(wallTrans);
	light->AddChild(wall0);
	wall0->AddChild(geomNode);
	phys->AddCollider(new QuadCollider(wallTrans));

	wallTrans = glm::mat4();
	wallTrans = glm::translate(wallTrans, glm::vec3(-50.0, 0.0, 0.0));
	wallTrans = glm::rotate(wallTrans, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0));
	wallTrans = glm::scale(wallTrans, glm::vec3(10.0f, 100.0f, 100.0f));
	TransformNode* wall1 = new TransformNode(wallTrans);
	light->AddChild(wall1);
	wall1->AddChild(geomNode);
	phys->AddCollider(new QuadCollider(wallTrans));

	wallTrans = glm::mat4();
	wallTrans = glm::translate(wallTrans, glm::vec3(0.0, 0.0, 50.0));
	wallTrans = glm::rotate(wallTrans, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0));
	wallTrans = glm::scale(wallTrans, glm::vec3(100.0f, 100.0f, 100.0f));
	TransformNode* wall2 = new TransformNode(wallTrans);
	light->AddChild(wall2);
	wall2->AddChild(geomNode);
	phys->AddCollider(new QuadCollider(wallTrans));

	wallTrans = glm::mat4();
	wallTrans = glm::translate(wallTrans, glm::vec3(0.0, 0.0, -50.0));
	wallTrans = glm::rotate(wallTrans, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0));
	wallTrans = glm::scale(wallTrans, glm::vec3(100.0f, 10.0f, 100.0f));
	TransformNode* wall3 = new TransformNode(wallTrans);
	light->AddChild(wall3);
	wall3->AddChild(geomNode);
	phys->AddCollider(new QuadCollider(wallTrans));

	//LIGHTS----------------
	TransformNode* lightNode = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.2, 0.2, 0.2)));
	TransformNode* lightScale = new TransformNode(glm::translate(glm::mat4(), glm::vec3(4, 4, 14)));



	//GEOMETRY--------------
	//Table
	//leg 1
	glm::mat4 legTrans = glm::mat4();
	TransformNode* leg1Trans = new TransformNode(glm::translate(legTrans, glm::vec3(-10, 1, 14)));
	TransformNode* leg1Scale = new TransformNode(glm::scale(legTrans, glm::vec3(0.2, 1.3, 0.2)));
	TexturedLit* leg = new TexturedLit(cube, "textures/wood_diffuse.jpg", "textures/wood_normal.png");
	GeometryNode* legNode = new GeometryNode(leg);
	light->AddChild(leg1Trans);
	leg1Trans->AddChild(leg1Scale);
	leg1Scale->AddChild(legNode);
	phys->AddCollider(new QuadCollider(legTrans));
	//leg 2
	legTrans = glm::mat4();
	TransformNode* leg2Trans = new TransformNode(glm::translate(legTrans, glm::vec3(-13, 1, 14)));
	TransformNode* leg2Scale = new TransformNode(glm::scale(legTrans, glm::vec3(0.2, 1.3, 0.2)));
	TexturedLit* leg2 = new TexturedLit(cube, "textures/wood_diffuse.jpg", "textures/wood_normal.png");
	GeometryNode* leg2Node = new GeometryNode(leg2);
	light->AddChild(leg2Trans);
	leg2Trans->AddChild(leg2Scale);
	leg2Scale->AddChild(leg2Node);
	phys->AddCollider(new QuadCollider(legTrans));
	//leg 3
	TransformNode* leg3Trans = new TransformNode(glm::translate(legTrans, glm::vec3(-10, 1, 17)));
	TransformNode* leg3Scale = new TransformNode(glm::scale(legTrans, glm::vec3(0.2, 1.3, 0.2)));
	TexturedLit* leg3 = new TexturedLit(cube, "textures/wood_diffuse.jpg", "textures/wood_normal.png");
	GeometryNode* leg3Node = new GeometryNode(leg3);
	light->AddChild(leg3Trans);
	leg3Trans->AddChild(leg3Scale);
	leg3Scale->AddChild(leg3Node);
	phys->AddCollider(new QuadCollider(legTrans));
	//leg 4
	TransformNode* leg4Trans = new TransformNode(glm::translate(legTrans, glm::vec3(-13, 1, 17)));
	TransformNode* leg4Scale = new TransformNode(glm::scale(legTrans, glm::vec3(0.2, 1.3, 0.2)));
	TexturedLit* leg4 = new TexturedLit(cube, "textures/wood_diffuse.jpg", "textures/wood_normal.png");
	GeometryNode* leg4Node = new GeometryNode(leg4);
	light->AddChild(leg4Trans);
	leg4Trans->AddChild(leg4Scale);
	leg4Scale->AddChild(leg4Node);
	phys->AddCollider(new QuadCollider(legTrans));
	//actual table
	TransformNode* tableTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(-11.4, 2.2, 15.4)));
	TransformNode* tableScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(2, 0.2, 2)));
	TexturedLit* table = new TexturedLit(cube, "textures/wood_diffuse.jpg", "textures/wood_normal.png");
	GeometryNode* tableNode = new GeometryNode(table);
	light->AddChild(tableTrans);
	tableTrans->AddChild(tableScale);
	tableScale->AddChild(tableNode);

	//various doors
	glm::mat4 doorMat = glm::mat4();
	doorMat = glm::translate(doorMat, glm::vec3(-48, 4, 24));
	doorMat = glm::scale(doorMat, glm::vec3(1, 8, 6));
	TransformNode* doorTrans = new TransformNode(doorMat);
	TexturedLit* door = new TexturedLit(cube, "textures/door.jpg");
	GeometryNode* doorNode = new GeometryNode(door);
	light->AddChild(doorTrans);
	doorTrans->AddChild(doorNode);



	//world map borders
	glm::mat4 borderM = glm::mat4();
	TransformNode* borderTrans = new TransformNode(glm::translate(borderM, glm::vec3(0, 0.5, 24)));
	TransformNode* borderScale = new TransformNode(glm::scale(borderM, glm::vec3(1, 0.5, 25)));
	TexturedLit* border = new TexturedLit(cube, "textures/ceramic.png");
	GeometryNode* borderNode = new GeometryNode(border);
	light->AddChild(borderTrans);
	borderTrans->AddChild(borderScale);
	borderScale->AddChild(borderNode);
	phys->AddCollider(new QuadCollider(borderM));

	borderM = glm::mat4();
	TransformNode* border2Trans = new TransformNode(glm::translate(borderM, glm::vec3(24, 0.5, 0)));
	TransformNode* border2Scale = new TransformNode(glm::scale(borderM, glm::vec3(25, 0.5, 1)));
	TexturedLit* border2 = new TexturedLit(cube, "textures/ceramic.png");
	GeometryNode* border2Node = new GeometryNode(border2);
	light->AddChild(border2Trans);
	border2Trans->AddChild(border2Scale);
	border2Scale->AddChild(border2Node);
	phys->AddCollider(new QuadCollider(borderM));

	borderM = glm::mat4();
	TransformNode* border3Trans = new TransformNode(glm::translate(borderM, glm::vec3( 11, 0.4, -20)));
	TransformNode* border3Scale = new TransformNode(glm::scale(borderM, glm::vec3(40, 10, 2)));
	TexturedLit* border3 = new TexturedLit(cube, "textures/ceramic.png");
	GeometryNode* border3Node = new GeometryNode(border3);
	light->AddChild(border3Trans);
	border3Trans->AddChild(border3Scale);
	border3Scale->AddChild(border3Node);


	borderM = glm::mat4();
	TransformNode* border4Trans = new TransformNode(glm::translate(borderM, glm::vec3(-48, 0.4, -20)));
	TransformNode* border4Scale = new TransformNode(glm::scale(borderM, glm::vec3(8, 10, 2)));
	TexturedLit* border4 = new TexturedLit(cube, "textures/arrow.jpg");
	GeometryNode* border4Node = new GeometryNode(border4);
	light->AddChild(border4Trans);
	border4Trans->AddChild(border4Scale);
	border4Scale->AddChild(border4Node);

	borderM = glm::mat4();
	TransformNode* border5Trans = new TransformNode(glm::translate(borderM, glm::vec3(-50, 0.4, 15)));
	TransformNode* border5Scale = new TransformNode(glm::scale(borderM, glm::vec3(2, 50, 40)));
	TexturedLit* border5 = new TexturedLit(cube, "textures/ceramic.png");
	GeometryNode* border5Node = new GeometryNode(border5);
	light->AddChild(border5Trans);
	border5Trans->AddChild(border5Scale);
	border5Scale->AddChild(border5Node);


	borderM = glm::mat4();
	TransformNode* border6Trans = new TransformNode(glm::translate(borderM, glm::vec3(0, 0.4, 50)));
	TransformNode* border6Scale = new TransformNode(glm::scale(borderM, glm::vec3(50, 50, 2)));
	TexturedLit* border6 = new TexturedLit(cube, "textures/ceramic.png");
	GeometryNode* border6Node = new GeometryNode(border6);
	light->AddChild(border6Trans);
	border6Trans->AddChild(border6Scale);
	border6Scale->AddChild(border6Node);

	borderM = glm::mat4();
	TransformNode* border7Trans = new TransformNode(glm::translate(borderM, glm::vec3(49, 0.4, -20)));
	TransformNode* border7Scale = new TransformNode(glm::scale(borderM, glm::vec3(5, 50, 5)));
	TexturedLit* border7 = new TexturedLit(cube, "textures/ceramic.png");
	GeometryNode* border7Node = new GeometryNode(border7);
	light->AddChild(border7Trans);
	border7Trans->AddChild(border7Scale);
	border7Scale->AddChild(border7Node);


	borderM = glm::mat4();
	TransformNode* border8Trans = new TransformNode(glm::translate(borderM, glm::vec3(49, 49, 15)));
	TransformNode* border8Scale = new TransformNode(glm::scale(borderM, glm::vec3(6, 4, 40)));
	TexturedLit* border8 = new TexturedLit(cube, "textures/ceramic.png");
	GeometryNode* border8Node = new GeometryNode(border8);
	light->AddChild(border8Trans);
	border8Trans->AddChild(border8Scale);
	border8Scale->AddChild(border8Node);

	


	//cupboards
	glm::mat4 cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(-48, 1, 15));
	cupMat = glm::scale(cupMat, glm::vec3(1, 4, 2));
	TransformNode* boxNode = new TransformNode(cupMat);
	TexturedLit* boxLit = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* box = new GeometryNode(boxLit);
	light->AddChild(boxNode);
	boxNode->AddChild(box);
	phys->AddCollider(new QuadCollider(cupMat));

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(-48, 1, 7));
	cupMat = glm::scale(cupMat, glm::vec3(1, 4, 2));
	TransformNode* box2Node = new TransformNode(cupMat);
	TexturedLit* box2Lit = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* box2 = new GeometryNode(box2Lit);
	light->AddChild(box2Node);
	box2Node->AddChild(box2);
	phys->AddCollider(new QuadCollider(cupMat));

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(-48, 1, -1));
	cupMat = glm::scale(cupMat, glm::vec3(1, 4, 2));
	TransformNode* box3Node = new TransformNode(cupMat);
	TexturedLit* box3Lit = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* box3 = new GeometryNode(box3Lit);
	light->AddChild(box3Node);
	box3Node->AddChild(box3);
	phys->AddCollider(new QuadCollider(cupMat));

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(-25, 0.3, 48));
	cupMat = glm::scale(cupMat, glm::vec3(20, 8, 4));
	TransformNode* box4Node = new TransformNode(cupMat);
	TexturedLit* box4Lit = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* box4 = new GeometryNode(box4Lit);
	light->AddChild(box4Node);
	box4Node->AddChild(box4);



	// the computers and pipes
	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(48, 0.4, -10));
	cupMat = glm::scale(cupMat, glm::vec3(3, 1.5, 8));
	TransformNode* deskNode = new TransformNode(cupMat);
	TexturedLit* deskLit = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* desk = new GeometryNode(deskLit);
	light->AddChild(deskNode);
	deskNode->AddChild(desk);

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(48, 1, -10));
	cupMat = glm::scale(cupMat, glm::vec3(0.4, 2.4, 1.6));
	TransformNode* monitorNode = new TransformNode(cupMat);
	TexturedLit* monLit = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* monitor = new GeometryNode(monLit);
	light->AddChild(monitorNode);
	monitorNode->AddChild(monitor);

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(47.65, 2.8, -10));
	cupMat = glm::scale(cupMat, glm::vec3(0.1, 0.6, 1));
	TransformNode* screenNode = new TransformNode(cupMat);
	TexturedLit* screenLit = new TexturedLit(cube, "textures/screen.jpg");
	GeometryNode* screen = new GeometryNode(screenLit);
	light->AddChild(screenNode);
	screenNode->AddChild(screen);

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(-0.8, 0.3, 4));
	cupMat = glm::scale(cupMat, glm::vec3(0.7, 2.1, 3));
	TransformNode* desk2Node = new TransformNode(cupMat);
	TexturedLit* desk2Lit = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* desk2 = new GeometryNode(desk2Lit);
	light->AddChild(desk2Node);
	desk2Node->AddChild(desk2);

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(-0.8, 1.8, 4));
	cupMat = glm::scale(cupMat, glm::vec3(0.8, 0.8, 0.8));
	cupMat = glm::rotate(cupMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TransformNode* standNode = new TransformNode(cupMat);
	TexturedLit* standLit = new TexturedLit(cup, "textures/screen.jpg");
	GeometryNode* stand = new GeometryNode(standLit);
	light->AddChild(standNode);
	standNode->AddChild(stand);

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(-49, 20, 5));
	cupMat = glm::scale(cupMat, glm::vec3(4, 4, 25));
	cupMat = glm::rotate(cupMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TransformNode* pipeNode = new TransformNode(cupMat);
	TexturedLit* pipeLit = new TexturedLit(cup, "textures/screen.jpg");
	GeometryNode* pipe = new GeometryNode(pipeLit);
	light->AddChild(pipeNode);
	pipeNode->AddChild(pipe);

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(-49, 30, 5));
	cupMat = glm::scale(cupMat, glm::vec3(4, 4, 25));
	cupMat = glm::rotate(cupMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TransformNode* pipe2Node = new TransformNode(cupMat);
	TexturedLit* pipe2Lit = new TexturedLit(cup, "textures/screen.jpg");
	GeometryNode* pipe2 = new GeometryNode(pipe2Lit);
	light->AddChild(pipe2Node);
	pipe2Node->AddChild(pipe2);

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(-49, 6, 35));
	cupMat = glm::scale(cupMat, glm::vec3(4, 11, 4));
	//cupMat = glm::rotate(cupMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TransformNode* pipe3Node = new TransformNode(cupMat);
	TexturedLit* pipe3Lit = new TexturedLit(cup, "textures/screen.jpg");
	GeometryNode* pipe3 = new GeometryNode(pipe3Lit);
	light->AddChild(pipe3Node);
	pipe3Node->AddChild(pipe3);

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(-49, 25, 35));
	cupMat = glm::scale(cupMat, glm::vec3(3, 6, 4));
	TransformNode* powerNode = new TransformNode(cupMat);
	TexturedLit* powerLit = new TexturedLit(cube, "textures/cupboard.jpg", "textures/stonenorms.png");
	GeometryNode* powerBox = new GeometryNode(powerLit);
	light->AddChild(powerNode);
	powerNode->AddChild(powerBox);

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(-47.5, 25, 34));
	cupMat = glm::scale(cupMat, glm::vec3(2, 1, 1));
	TransformNode* switchNode = new TransformNode(cupMat);
	Emissive* switchLit = new Emissive(cube, glm::vec4(1.0f,0.0f,0.0f,1.0f));
	GeometryNode* switchBox = new GeometryNode(switchLit);
	light->AddChild(switchNode);
	switchNode->AddChild(switchBox);

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(-47.5, 25, 37));
	cupMat = glm::scale(cupMat, glm::vec3(2, 1, 1));
	TransformNode* switch2Node = new TransformNode(cupMat);
	Emissive* switch2Lit = new Emissive(cube, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	GeometryNode* switch2Box = new GeometryNode(switch2Lit);
	light->AddChild(switch2Node);
	switch2Node->AddChild(switch2Box);

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(0, 0.4, -17));
	cupMat = glm::scale(cupMat, glm::vec3(15, 0.8, 6));
	TransformNode* deNode = new TransformNode(cupMat);
	TexturedLit* deLit = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* deBox = new GeometryNode(deLit);
	light->AddChild(deNode);
	deNode->AddChild(deBox);

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(0, 2, -17));
	cupMat = glm::scale(cupMat, glm::vec3(7, 3, 2));
	TransformNode* moniNode = new TransformNode(cupMat);
	TexturedLit* moniLit = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* moniBox = new GeometryNode(moniLit);
	light->AddChild(moniNode);
	moniNode->AddChild(moniBox);

	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(0, 3, -15.1));
	cupMat = glm::scale(cupMat, glm::vec3(5, 2, 0.8));
	TransformNode* screen2Node = new TransformNode(cupMat);
	TexturedLit* screen2Lit = new TexturedLit(cube, "textures/screen.jpg");
	GeometryNode* screen2 = new GeometryNode(screen2Lit);
	light->AddChild(screen2Node);
	screen2Node->AddChild(screen2);



	//Terrain
	//building equipment
	glm::mat4 craneM = glm::mat4();
	craneM = glm::translate(craneM, glm::vec3(13, 2, 16));
	craneM = glm::scale(craneM, glm::vec3(0.4, 14, 0.4));
	TransformNode* craneNode = new TransformNode(craneM);
	TexturedLit* craneLit = new TexturedLit(cube, "textures/pebble.jpg");
	GeometryNode* crane = new GeometryNode(craneLit);
	light->AddChild(craneNode);
	craneNode->AddChild(crane);
	//c2
	craneM = glm::mat4();
	craneM = glm::translate(craneM, glm::vec3(12.7, 13, 16));
	craneM = glm::scale(craneM, glm::vec3(0.6, 0.6, 0.6));
	TransformNode* pilotNode = new TransformNode(craneM);
	TexturedLit* pilotLit = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* pilot = new GeometryNode(pilotLit);
	light->AddChild(pilotNode);
	pilotNode->AddChild(pilot);
	//cr3
	craneM = glm::mat4();
	craneM = glm::translate(craneM, glm::vec3(12.7, 13.4, 16));
	craneM = glm::scale(craneM, glm::vec3(0.7, 0.7, 0.7));
	craneM = glm::rotate(craneM, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TransformNode* rodNode = new TransformNode(craneM);
	TexturedLit* rodLit = new TexturedLit(cup, "textures/cupboard.jpg");
	GeometryNode* rod = new GeometryNode(rodLit);
	light->AddChild(rodNode);
	rodNode->AddChild(rod);
	//c4
	craneM = glm::mat4();
	craneM = glm::translate(craneM, glm::vec3(13, 14.7, 19));
	craneM = glm::scale(craneM, glm::vec3(0.4, 0.4, 4));
	craneM = glm::rotate(craneM, glm::radians(60.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	TransformNode* bridNode = new TransformNode(craneM);
	TexturedLit* bridLit = new TexturedLit(cube, "textures/pebble.jpg");
	GeometryNode* bridge = new GeometryNode(bridLit);
	light->AddChild(bridNode);
	bridNode->AddChild(bridge);

	//crane 2
	//c1
	craneM = glm::mat4();
	craneM = glm::translate(craneM, glm::vec3(14, 2, 11));
	craneM = glm::scale(craneM, glm::vec3(0.4, 10, 0.4));
	TransformNode* crane2Node = new TransformNode(craneM);
	TexturedLit* crane2Lit = new TexturedLit(cube, "textures/pebble.jpg");
	GeometryNode* crane2 = new GeometryNode(craneLit);
	light->AddChild(crane2Node);
	crane2Node->AddChild(crane2);
	//c2
	craneM = glm::mat4();
	craneM = glm::translate(craneM, glm::vec3(14, 9.5, 11));
	craneM = glm::scale(craneM, glm::vec3(0.6, 0.6, 0.6));
	TransformNode* pilot2Node = new TransformNode(craneM);
	TexturedLit* pilot2Lit = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* pilot2 = new GeometryNode(pilot2Lit);
	light->AddChild(pilot2Node);
	pilot2Node->AddChild(pilot2);
	//cr3
	craneM = glm::mat4();
	craneM = glm::translate(craneM, glm::vec3(14, 9.8, 11));
	craneM = glm::scale(craneM, glm::vec3(0.7, 0.7, 0.7));
	craneM = glm::rotate(craneM, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TransformNode* rod2Node = new TransformNode(craneM);
	TexturedLit* rod2Lit = new TexturedLit(cup, "textures/cupboard.jpg");
	GeometryNode* rod2 = new GeometryNode(rod2Lit);
	light->AddChild(rod2Node);
	rod2Node->AddChild(rod2);
	//c4
	craneM = glm::mat4();
	craneM = glm::translate(craneM, glm::vec3(18, 11, 11));
	craneM = glm::scale(craneM, glm::vec3(5, 0.4, 0.4));
	TransformNode* bridgeNode2 = new TransformNode(craneM);
	TexturedLit* bridgeLit2 = new TexturedLit(cube, "textures/pebble.jpg");
	GeometryNode* bridge2 = new GeometryNode(bridgeLit2);
	light->AddChild(bridgeNode2);
	bridgeNode2->AddChild(bridge2);
	



	//terrain geometry
	glm::mat4 shelters = glm::mat4();
	shelters = glm::translate(shelters, glm::vec3(7, 0.38, 16));
	shelters = glm::scale(shelters, glm::vec3(0.4, 0.4, 0.4));
	shelters = glm::rotate(shelters, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TransformNode* shelNode = new TransformNode(shelters);
	TexturedLit* shelLit = new TexturedLit(cup, "textures/cupboard.jpg");
	GeometryNode* shelter = new GeometryNode(shelLit);
	light->AddChild(shelNode);
	shelNode->AddChild(shelter);

	shelters = glm::mat4();
	shelters = glm::translate(shelters, glm::vec3(6, 0.38, 16));
	shelters = glm::scale(shelters, glm::vec3(0.4, 0.4, 0.4));
	shelters = glm::rotate(shelters, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TransformNode* shel2Node = new TransformNode(shelters);
	TexturedLit* shelLit2 = new TexturedLit(cup, "textures/cupboard.jpg");
	GeometryNode* shelter2 = new GeometryNode(shelLit2);
	light->AddChild(shel2Node);
	shel2Node->AddChild(shelter2);


	shelters = glm::mat4();
	shelters = glm::translate(shelters, glm::vec3(5, 0.38, 16));
	shelters = glm::scale(shelters, glm::vec3(0.4, 0.4, 0.4));
	shelters = glm::rotate(shelters, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TransformNode* shel3Node = new TransformNode(shelters);
	TexturedLit* shelLit3 = new TexturedLit(cup, "textures/cupboard.jpg");
	GeometryNode* shelter3 = new GeometryNode(shelLit3);
	light->AddChild(shel3Node);
	shel3Node->AddChild(shelter3);

	shelters = glm::mat4();
	shelters = glm::translate(shelters, glm::vec3(5, 0.38, 20));
	shelters = glm::scale(shelters, glm::vec3(0.8, 0.6, 0.8));
	shelters = glm::rotate(shelters, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TransformNode* shel4Node = new TransformNode(shelters);
	TexturedLit* shelLit4 = new TexturedLit(cup, "textures/cupboard.jpg");
	GeometryNode* shelter4 = new GeometryNode(shelLit4);
	light->AddChild(shel4Node);
	shel4Node->AddChild(shelter4);

	shelters = glm::mat4();
	shelters = glm::translate(shelters, glm::vec3(4, 0.38, 20));
	shelters = glm::scale(shelters, glm::vec3(0.3, 0.4, 0.3));
	//shelters = glm::rotate(shelters, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TransformNode* shel5Node = new TransformNode(shelters);
	TexturedLit* shelLit5 = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* shelter5 = new GeometryNode(shelLit5);
	light->AddChild(shel5Node);
	shel5Node->AddChild(shelter5);

	shelters = glm::mat4();
	shelters = glm::translate(shelters, glm::vec3(4, 0.38, 22));
	shelters = glm::scale(shelters, glm::vec3(0.6, 0.2, 0.2));
	//shelters = glm::rotate(shelters, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TransformNode* shel6Node = new TransformNode(shelters);
	TexturedLit* shelLit6 = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* shelter6 = new GeometryNode(shelLit6); 
	light->AddChild(shel6Node);
	shel6Node->AddChild(shelter6);

	shelters = glm::mat4();
	shelters = glm::translate(shelters, glm::vec3(2.5, 0.38, 19));
	shelters = glm::scale(shelters, glm::vec3(0.3, 0.3, 0.5));
	//shelters = glm::rotate(shelters, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TransformNode* shel7Node = new TransformNode(shelters);
	TexturedLit* shelLit7 = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* shelter7 = new GeometryNode(shelLit7);
	light->AddChild(shel7Node);
	shel7Node->AddChild(shelter7);

	shelters = glm::mat4();
	shelters = glm::translate(shelters, glm::vec3(2.5, 0.18, 24.85));
	shelters = glm::scale(shelters, glm::vec3(0.3, 0.3, 0.5));
	shelters = glm::rotate(shelters, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	TransformNode* shel8Node = new TransformNode(shelters);
	TexturedLit* shelLit8 = new TexturedLit(cup, "textures/cupboard.jpg");
	GeometryNode* shelter8 = new GeometryNode(shelLit8);
	light->AddChild(shel8Node);
	shel8Node->AddChild(shelter8);

	shelters = glm::mat4();
	shelters = glm::translate(shelters, glm::vec3(3.5, 0.18, 24.85));
	shelters = glm::scale(shelters, glm::vec3(0.3, 0.3, 0.5));
	shelters = glm::rotate(shelters, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	TransformNode* shel9Node = new TransformNode(shelters);
	TexturedLit* shelLit9 = new TexturedLit(cube, "textures/cupboard.jpg");
	GeometryNode* shelter9 = new GeometryNode(shelLit9);
	light->AddChild(shel9Node);
	shel9Node->AddChild(shelter9);




	//MESHES----------------
	//Nodes for imported meshes - the scales
	TransformNode* rockScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(10, 10, 10)));
	TransformNode* rock2Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(6, 6, 6)));
	TransformNode* rock3Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(5, 5, 5)));
	TransformNode* rock4Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(8, 8, 8)));
	TransformNode* rock5Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(4, 4, 4)));
	TransformNode* rock6Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.2, 4, 0.2)));
	TransformNode* rock7Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.2, 4, 0.2)));
	TransformNode* bagelScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(125, 125, 125)));
	TransformNode* mmScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(6, 7, 4)));
	TransformNode* mm2Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(2, 3, 2)));
	TransformNode* mm3Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.7, 8, 0.7)));
	TransformNode* mm4Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(3, 3, 3)));
	TransformNode* mm5Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(6, 7, 4)));
	TransformNode* mm6Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(5, 5, 5)));
	TransformNode* mm7Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(6, 3, 6)));
	TransformNode* cupScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.3, 0.3, 0.3)));
	
	//Nodes for the imported meshes - the transforms
	TransformNode* rockTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(10, 0.5, 18)));
	TransformNode* rock2Trans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(5, 0.5, 18)));
	TransformNode* rock3Trans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(2, 0.1, 17)));
	TransformNode* rock4Trans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(2, 0.2, 39.5)));
	TransformNode* rock5Trans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(9, 0.2, 34)));
	TransformNode* rock6Trans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(8, 0.4, 13)));
	TransformNode* bagelTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(-25, 20, 50)));
	TransformNode* mmTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(8, 0.2, 41.5)));
	TransformNode* mm2Trans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(2, 0.2, 14)));
	TransformNode* mm3Trans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(4, 0.2, 14)));
	TransformNode* mm4Trans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(2, 0.2, 29.5)));
	TransformNode* mm5Trans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(8, 0.2, 13)));
	TransformNode* mm6Trans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(4, 0.2, 29)));
	TransformNode* mm7Trans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(4.5, 0.2, 28.5)));
	TransformNode* cupTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(-10.1, 2.7, 15.3)));

	//Nodes for the imported meshes - the rotations
	TransformNode* mm2Rot = new TransformNode(glm::rotate(glm::mat4(), glm::radians(65.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	TransformNode* bagelRot = new TransformNode(glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

	//Rocks
	//1
	light->AddChild(rockTrans);
	rockTrans->AddChild(rockScale);
	rockScale->AddChild(new GeometryNode(rock));
	//2
	light->AddChild(rock2Trans);
	rock2Trans->AddChild(rock2Scale);
	rock2Scale->AddChild(new GeometryNode(rock));
	//3
	light->AddChild(rock3Trans);
	rock3Trans->AddChild(rock3Scale);
	rock3Scale->AddChild(new GeometryNode(rock));
	//4
	light->AddChild(rock4Trans);
	rock4Trans->AddChild(rock4Scale);
	rock4Scale->AddChild(new GeometryNode(rock));
	//5
	light->AddChild(rock5Trans);
	rock5Trans->AddChild(rock5Scale);
	rock5Scale->AddChild(new GeometryNode(rock));
	//6
	light->AddChild(rock6Trans);
	rock6Trans->AddChild(rock6Scale);
	rock6Scale->AddChild(new GeometryNode(rock));


	//Hoop foods
	light->AddChild(bagelTrans);
	bagelTrans->AddChild(bagelScale);
	bagelScale->AddChild(new GeometryNode(bagel));

	

	//Man-made objects
	light->AddChild(mmTrans);
	mmTrans->AddChild(mmScale);
	mmScale->AddChild(new GeometryNode(man_made));
	//2
	light->AddChild(mm2Trans);
	mm2Trans->AddChild(mm2Scale); 
	mm2Scale->AddChild(new GeometryNode(man_made));
	//3
	light->AddChild(mm3Trans);
	mm3Trans->AddChild(streetlight);
	mm3Trans->AddChild(mm3Scale);
	mm3Scale->AddChild(new GeometryNode(man_made));
	mm3Scale->AddChild(new GeometryNode(bagelN));
	//4
	light->AddChild(cupTrans);
	TexturedLit* plastic = new TexturedLit(cup, "textures/ceramic.png");
	GeometryNode* cupNode = new GeometryNode(plastic);
	cupTrans->AddChild(cupScale);
	cupScale->AddChild(cupNode);
	//5
	light->AddChild(mm4Trans);
	mm4Trans->AddChild(mm4Scale);
	mm4Scale->AddChild(new GeometryNode(man_made));
	//6
	light->AddChild(mm5Trans);
	mm5Trans->AddChild(mm5Scale);
	mm5Scale->AddChild(new GeometryNode(man_made));
	//7
	light->AddChild(mm6Trans);
	mm6Trans->AddChild(mm6Scale);
	mm6Scale->AddChild(new GeometryNode(man_made));
	//8
	light->AddChild(mm7Trans);
	mm7Trans->AddChild(mm7Scale);
	mm7Scale->AddChild(new GeometryNode(man_made));



	//The Solar System podium in the rocket laboratory
	//the podium itself
	cupMat = glm::mat4();
	cupMat = glm::translate(cupMat, glm::vec3(-30, 0.2, 25));
	cupMat = glm::scale(cupMat, glm::vec3(3, 1.5, 3));
	TransformNode* podNode = new TransformNode(cupMat);
	TexturedLit* podLit = new TexturedLit(cup, "textures/ceramic.png");
	GeometryNode* podium = new GeometryNode(podLit);
	light->AddChild(podNode);
	podNode->AddChild(podium);

	//the planets we are using
	TexturedLit* earth = new TexturedLit(sphere, "textures/2k_earth_daymap.jpg");
	TexturedLit* moon = new TexturedLit(sphere, "textures/2k_moon.jpg");
	TexturedLit* venus = new TexturedLit(sphere, "textures/2k_venus.jpg");
	TexturedLit* mars = new TexturedLit(sphere, "textures/2k_mars.jpg");
	TexturedLit* moon2 = new TexturedLit(sphere, "textures/2k_moon.jpg");
	TexturedLit* jupiter = new TexturedLit(sphere, "textures/2k_jupiter.jpg");
	Emissive* sun = new Emissive(sphere, glm::vec4(1.0f,1.0f,0.5f,1.0f));

	//scales for the planets
	TransformNode* sunTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(-30, 2.4, 25)));
	TransformNode* sunScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.5, 0.5, 0.5)));

	TransformNode* earthScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.1, 0.1, 0.1 )));
	TransformNode* earthOrbit = new TransformNode(glm::mat4());
	TransformNode* earthRot = new TransformNode(glm::mat4());
	TransformNode* earthTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(1.3, 1.3, 1.3)));

	TransformNode* moonTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(1, 0, 0)));
	TransformNode* moonScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.05, 0.05, 0.05)));
	TransformNode* moonOrbit = new TransformNode(glm::mat4());

	TransformNode* venusTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(2, 1, 0)));
	TransformNode* venusScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.4, 0.4, 0.4)));
	TransformNode* venusRot = new TransformNode(glm::mat4());
	TransformNode* venusOrbit = new TransformNode(glm::mat4());

	TransformNode* marsTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(3, 0, 0)));
	TransformNode* marsScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.35)));
	TransformNode* marsRot = new TransformNode(glm::mat4());
	TransformNode* marsOrbit = new TransformNode(glm::mat4());

	TransformNode* jupTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(5, 0, 0)));
	TransformNode* jupScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.7, 0.7, 0.7)));
	TransformNode* jupRot = new TransformNode(glm::mat4());
	TransformNode* jupOrbit = new TransformNode(glm::mat4());

	TransformNode* moon2Trans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(1, 0.9, 0)));
	TransformNode* moon2Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.2, 0.2, 0.2)));
	TransformNode* moon2Orbit = new TransformNode(glm::mat4());




	//scene graph for the solar system diorama
	light->AddChild(sunTrans);
	sunTrans->AddChild(sunScale);
	sunScale->AddChild(new GeometryNode(sun));

	sunScale->AddChild(earthOrbit);
	earthOrbit->AddChild(earthTrans);
	earthOrbit->SetUpdateCallback(new RotatorCallback(earthOrbit, 1.0f));

	
	earthTrans->AddChild(moonOrbit);
	moonOrbit->AddChild(moonTrans);
	moonOrbit->SetUpdateCallback(new RotatorCallback(moonOrbit, 0.5f));
	moonTrans->AddChild(moonScale);
	moonScale->AddChild(new GeometryNode(moon));

	
	earthTrans->AddChild(earthRot);
	earthRot->AddChild(earthScale);
	earthRot->SetUpdateCallback(new RotatorCallback(earthRot, 3.0f));
	earthScale->AddChild(new GeometryNode(earth));

	sunScale->AddChild(venusOrbit);
	venusOrbit->AddChild(venusTrans);
	venusOrbit->SetUpdateCallback(new RotatorCallback(venusOrbit,0.4f));
	venusTrans->AddChild(venusRot);
	venusRot->AddChild(venusScale);
	venusRot->SetUpdateCallback(new RotatorCallback(venusRot, 2.0f));
	venusScale->AddChild(new GeometryNode(venus));

	sunScale->AddChild(marsOrbit);
	marsOrbit->AddChild(marsTrans);
	marsOrbit->SetUpdateCallback(new RotatorCallback(marsOrbit, 0.4f));
	marsTrans->AddChild(marsRot);
	marsRot->AddChild(marsScale);
	marsRot->SetUpdateCallback(new RotatorCallback(marsRot, 2.0f));
	marsScale->AddChild(new GeometryNode(mars));

	sunScale->AddChild(jupOrbit);
	jupOrbit->AddChild(jupTrans);
	jupOrbit->SetUpdateCallback(new RotatorCallback(jupOrbit, 0.8f));
	jupTrans->AddChild(jupRot);
	jupRot->AddChild(jupScale);
	jupRot->SetUpdateCallback(new RotatorCallback(jupRot, 2.0f));
	jupScale->AddChild(new GeometryNode(jupiter));

	jupTrans->AddChild(moon2Orbit);
	moon2Orbit->AddChild(moon2Trans);
	moon2Orbit->SetUpdateCallback(new RotatorCallback(moon2Orbit, 0.5f));
	moon2Trans->AddChild(moon2Scale);
	moon2Scale->AddChild(new GeometryNode(moon2));


	
	

	//SPLINES----------
	//adding the poly line graphics
	//s1
	std::vector<glm::vec3> lineVerts;
	splinePoints.push_back(glm::vec3(39, 3, 0));
	splinePoints.push_back(glm::vec3(38, 2, 0));
	splinePoints.push_back(glm::vec3(42, 2, 20));
	splinePoints.push_back(glm::vec3(41, 2, 30));
	splinePoints.push_back(glm::vec3(40, 2, 40));
	splinePoints.push_back(glm::vec3(43, 3, 20));
	//s2
	std::vector<glm::vec3> crVerts;
	spline2Points.push_back(glm::vec3(10, 18, 0));
	spline2Points.push_back(glm::vec3(13, 18, 10));
	spline2Points.push_back(glm::vec3(15, 18, 23));
	spline2Points.push_back(glm::vec3(19, 18, 25));
	spline2Points.push_back(glm::vec3(25, 18, 30));
	spline2Points.push_back(glm::vec3(19, 18, 31));
	spline2Points.push_back(glm::vec3(15, 18, 27));
	spline2Points.push_back(glm::vec3(44, 18, 37));
	//s3
	std::vector<glm::vec3> aVerts;
	spline3Points.push_back(glm::vec3(0, 30, 0));
	spline3Points.push_back(glm::vec3(43, 30, 23));
	spline3Points.push_back(glm::vec3(44, 30, 29));
	spline3Points.push_back(glm::vec3(39, 30, 34));
	spline3Points.push_back(glm::vec3(35, 30, 37));
	spline3Points.push_back(glm::vec3(25, 30, 31));
	spline3Points.push_back(glm::vec3(19, 30, 26));
	spline3Points.push_back(glm::vec3(18, 30, 14));
	//s4
	std::vector<glm::vec3> pVerts;
	spline4Points.push_back(glm::vec3(-10.5, 3.4, 15.3));
	spline4Points.push_back(glm::vec3(-10.45, 3.25, 15.3));
	spline4Points.push_back(glm::vec3(-10.35, 3, 15.3));
	spline4Points.push_back(glm::vec3(-10.3, 2.8, 15.3));
	spline4Points.push_back(glm::vec3(-10.2, 2.7, 15.3));
	spline4Points.push_back(glm::vec3(-10.2, 2.8, 15.3));
	spline4Points.push_back(glm::vec3(-10.2, 3, 15.3));
	spline4Points.push_back(glm::vec3(-10.45, 3.25, 15.3));

	//if we want it to loop, have boolean value as true
	//this will ensure the spline meets itself and you can have a revolving
	//animation.
	//CR SPLINE-----------
	CRSpline c(splinePoints, true);
	for (int i = 0; i < 800; i++)
	{
		float u = 8.0f * i / 799.0f;
		lineVerts.push_back(c.GetPoint(u));
	}

	//the polyline reference
	PolyLine* p = new PolyLine(lineVerts, glm::vec4(1, 0, 0, 1));
	light->AddChild(new GeometryNode(p));

	TransformNode* patchTrans = new TransformNode(glm::scale(glm::translate(glm::mat4(), glm::vec3(0, 20, 0)), glm::vec3(5, 5, 5)));
	//TexturedLit* patch = new TexturedLit(BezierPatch(bezPoints, 8), "textures/ceramic.png");
	TexturedLit* patch = new TexturedLit(Rocket(8), "terrain_textures/metal.jpg");
	GeometryNode* rNode = new GeometryNode(patch);
	light->AddChild(patchTrans);
	patchTrans->AddChild(rNode);
	patchTrans->SetUpdateCallback(new SplineAnimationCallback(patchTrans, new CRSpline(splinePoints, true, 0.5f), 0.5f));

	//2 SPLINE--------------
	BSpline b(spline2Points, true);
	for (int i = 0; i < 800; i++)
	{
		float s = 8.0f * i / 799.0f;
		crVerts.push_back(b.GetPoint(s));

	}

	PolyLine* p2 = new PolyLine(crVerts, glm::vec4(0, 1, 0, 1));
	light->AddChild(new GeometryNode(p2));

	TransformNode* patch2Trans = new TransformNode(glm::scale(glm::translate(glm::mat4(), glm::vec3(0, 20, 0)), glm::vec3(5, 5, 5)));
	//TexturedLit* patch = new TexturedLit(BezierPatch(bezPoints, 8), "textures/ceramic.png");
	TexturedLit* patch2 = new TexturedLit(Rocket(8), "textures/cage.jpg");
	GeometryNode* r2Node = new GeometryNode(patch2);
	light->AddChild(patch2Trans);
	patch2Trans->AddChild(r2Node);
	patch2Trans->SetUpdateCallback(new SplineAnimationCallback(patch2Trans, new BSpline(spline2Points, true), 1.0f));


	//THIRD SPLINE---------------
	BSpline sp(spline3Points, true);
	for (int i = 0; i < 800; i++)
	{
		float d = 8.0f * i / 799.0f;
		aVerts.push_back(sp.GetPoint(d));
	}

	PolyLine* p3 = new PolyLine(aVerts, glm::vec4(0, 0, 1, 1));
	light->AddChild(new GeometryNode(p3));

	//SPAWNING OBJECTS TO DENOTE CONTROL POINTS
	SphereGeometry cyl(8);
	TexturedLit* cylRender = new TexturedLit(cyl, "textures/cage.jpg");
	GeometryNode* cylNode = new GeometryNode(cylRender);

	for (glm::vec3 curPos : spline3Points)
	{
		TransformNode* cylTrans = new TransformNode(glm::translate(glm::mat4(), curPos));
		light->AddChild(cylTrans);
		cylTrans->AddChild(cylNode);
	}

	TransformNode* patch3Trans = new TransformNode(glm::scale(glm::translate(glm::mat4(), glm::vec3(0, 20, 0)), glm::vec3(6, 6, 6)));
	TexturedLit* patch3 = new TexturedLit(Rocket(8), "textures/cage.jpg");
	GeometryNode* r3Node = new GeometryNode(patch3);
	light->AddChild(patch3Trans);
	patch3Trans->AddChild(r3Node);
	patch3Trans->SetUpdateCallback(new SplineAnimationCallback(patch3Trans, new BSpline(spline3Points, true), 1.0f));


	//FOURTH SPLINE---------
	CRSpline pSp(spline4Points, true);
	for (int i = 0; i < 800; i++)
	{
		float y = 8.0f / 799.0f;
		pVerts.push_back(pSp.GetPoint(y));
	}

	PolyLine* p4 = new PolyLine(pVerts, glm::vec4(1, 1, 1, 1));// we don't want to see this spline so it has all 1,1,1,1 values
	light->AddChild(new GeometryNode(p4));

	//These transforms will make our 'pouring tea' effect
	TransformNode* patch4Trans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(-11.5, 2.9, 15.3)));
	TransformNode* patch4Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.05, 0.05, 0.05)));
	TexturedLit* drops = new TexturedLit(cube, "textures/stone_cobble.png", "textures/stonenorms.png");
	GeometryNode* dNode = new GeometryNode(drops);
	light->AddChild(patch4Trans);
	patch4Trans->AddChild(patch4Scale);
	patch4Scale->AddChild(dNode);
	patch4Trans->SetUpdateCallback(new SplineAnimationCallback(patch4Trans, new CRSpline(spline4Points, true, 0.5f), 0.2f));

	TransformNode* patch5Trans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(-11.5, 2.9, 15.3)));
	TransformNode* patch5Scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.04, 0.04, 0.04)));
	TexturedLit* drops1 = new TexturedLit(cube, "textures/stone_cobble.png", "textures/stonenorms.png");
	GeometryNode* dNode1 = new GeometryNode(drops1);
	light->AddChild(patch5Trans);
	patch5Trans->AddChild(patch5Scale);
	patch5Scale->AddChild(dNode1);
	patch5Trans->SetUpdateCallback(new SplineAnimationCallback(patch5Trans, new CRSpline(spline4Points, true, 0.5f), 0.6f));

	

	//vector points which represents the bezier patch rocket
	//the bezier patch rocket will move along the splines instead of the sphere
	std::vector<glm::vec3> bezPoints{
	glm::vec3(0,0,0), glm::vec3(1,1,0), glm::vec3(2,1,0), glm::vec3(3,0,0),
	glm::vec3(0,0,1), glm::vec3(1,2,1), glm::vec3(2,2,1), glm::vec3(3,0,1),
	glm::vec3(0,0,2), glm::vec3(1,2,2), glm::vec3(2,2,2), glm::vec3(3,0,2),
	glm::vec3(0,0,3), glm::vec3(1,1,3), glm::vec3(2,1,3), glm::vec3(3,0,3),
	};


	//the large bezier patch rocket - this will not move anywhere
	TransformNode* rocketTrans = new TransformNode(glm::scale(glm::translate(glm::mat4(), glm::vec3(18.0f, 19.0f, 15.0f)), glm::vec3(15, 15, 15)));
	TexturedLit* rocket = new TexturedLit(Rocket(8), "textures/stone_cobble.png", "textures/stonenorms.png");
	GeometryNode* rtNode = new GeometryNode(rocket);
	light->AddChild(rocketTrans);
	rocketTrans->AddChild(rtNode);

	//the bezier patch tea pot - this will also not move anywhere
	TransformNode* teapotTrans = new TransformNode(glm::scale(glm::translate(glm::mat4(), glm::vec3(-11.9, 2.2, 15.3)), glm::vec3(0.45, 0.45, 0.45)));
	TexturedLit* teapot = new TexturedLit(Teapot(8), "textures/ceramic.png", "textures/stonenorms.png");
	GeometryNode* tpNode = new GeometryNode(teapot);
	light->AddChild(teapotTrans);
	teapotTrans->AddChild(tpNode);

	
	
	return light;
}
