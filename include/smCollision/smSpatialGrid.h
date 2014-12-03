/*
****************************************************
				SimMedTK LICENSE
****************************************************

****************************************************
*/

#ifndef SMSPATIALGRID_H
#define SMSPATIALGRID_H

#include <Qthread>
#include <Qvector>
#include "smConfig.h"
#include "smConfigRendering.h"
#include "smCollisionDetection.h"
#include "smCustomRenderer.h"
#include "smScene.h"
#include "smStaticSceneObject.h"
#include "smSynchronization.h"
#include "smWorkerThread.h"
#include "smLattice.h"
#include "smLatticeTypes.h"
#include "smMemoryBlock.h"
#include "smSDK.h"
#include "smGLRenderer.h"
#include "smGeometry.h"

#define SOFMIS_SPATIALGRID_LEFTCORNER	smVec3<smFloat>(-10,-10,-10)
#define SOFMIS_SPATIALGRID_RIGHTCORNER  smVec3<smFloat>(10,10,10)
#define SOFMIS_SPATIALGRID_XSEPERATION 10
#define SOFMIS_SPATIALGRID_YSEPERATION 10
#define SOFMIS_SPATIALGRID_ZSEPERATION 10
#define SOFMIS_SPATIALGRID_WORKER_COLLISIONPAIRS	1000
#define SOFMIS_SPATIALGRID_TOTALLATTICES	500

/// \brief !!
class smSpatialGridWorker:public smWorkerThread{

	smLattice *latticePair;
	smLattice *latticePair2;

public:
	smMemoryBlock collisionPairs;
	smCollidedTriangles *pairs;
	smInt collidedPairs;

	///this is public for now
	smLattice *latticeList[SOFMIS_SPATIALGRID_TOTALLATTICES];
	smInt totalLattices;

	/// \brief destructor
	~smSpatialGridWorker(){
		collisionPairs.deleteMemory(QString("pairs"));
	}

	/// \brief constructor
	smSpatialGridWorker(){
		totalLattices=0;
		collidedPairs=0;
		collisionPairs.allocate<smCollidedTriangles>(QString("pairs"),SOFMIS_SPATIALGRID_WORKER_COLLISIONPAIRS,&pairs);
	}

	/// \brief constructor
	smSpatialGridWorker(smProcessID p_ID):smWorkerThread(p_ID){
		totalLattices=0;
		collisionPairs.allocate<smCollidedTriangles>(QString("pairs"),SOFMIS_SPATIALGRID_WORKER_COLLISIONPAIRS,&pairs);
	}

	/// \brief !!
	inline void checkNarrow(smLattice *p_latticeA, smLattice *p_latticeB, smInt p_primAIndex,smInt p_primBIndex){

		smInt coPlanar;
		smVec3<smFloat> interSectPoint1;
		smVec3<smFloat> interSectPoint2;
		smVec3<smFloat> projectedPoint1;
		smVec3<smFloat> projectedPoint2;
		smShort singlePoint1;
		smShort singlePoint2;

		if(smCollisionUtils::tri2tri(p_latticeA->mesh->vertices[p_latticeA->mesh->triangles[p_primAIndex].vert[0]],
								   p_latticeA->mesh->vertices[p_latticeA->mesh->triangles[p_primAIndex].vert[1]],
								   p_latticeA->mesh->vertices[p_latticeA->mesh->triangles[p_primAIndex].vert[2]],
								   p_latticeB->mesh->vertices[p_latticeB->mesh->triangles[p_primBIndex].vert[0]],
								   p_latticeB->mesh->vertices[p_latticeB->mesh->triangles[p_primBIndex].vert[1]],
								   p_latticeB->mesh->vertices[p_latticeB->mesh->triangles[p_primBIndex].vert[2]],
								   coPlanar,
								   interSectPoint1,
								   interSectPoint2,
								   singlePoint1,
								   singlePoint2,
								   projectedPoint1,
								   projectedPoint2)){

			pairs[collidedPairs].point1=singlePoint1;
			pairs[collidedPairs].point2=singlePoint2;

			pairs[collidedPairs].proj1=projectedPoint1;
			pairs[collidedPairs].proj2=projectedPoint2;

			pairs[collidedPairs].tri1.meshID=p_latticeA->mesh->uniqueId;
			pairs[collidedPairs].tri2.meshID=p_latticeB->mesh->uniqueId;

			pairs[collidedPairs].tri1.primID=p_primAIndex;
			pairs[collidedPairs].tri2.primID=p_primBIndex;
			pairs[collidedPairs].tri1.vert[0]=p_latticeA->mesh->vertices [p_latticeA->mesh->triangles[p_primAIndex].vert[0]];
			pairs[collidedPairs].tri1.vert[1]=p_latticeA->mesh->vertices [p_latticeA->mesh->triangles[p_primAIndex].vert[1]];
			pairs[collidedPairs].tri1.vert[2]=p_latticeA->mesh->vertices [p_latticeA->mesh->triangles[p_primAIndex].vert[2]];

			pairs[collidedPairs].tri2.vert[0]=p_latticeB->mesh->vertices [p_latticeB->mesh->triangles[p_primBIndex].vert[0]];
			pairs[collidedPairs].tri2.vert[1]=p_latticeB->mesh->vertices [p_latticeB->mesh->triangles[p_primBIndex].vert[1]];
			pairs[collidedPairs].tri2.vert[2]=p_latticeB->mesh->vertices [p_latticeB->mesh->triangles[p_primBIndex].vert[2]];

			collidedPairs++;
		 }
	}

	/// \brief !!
	inline void checkCells(smLattice *p_latticeA,smLattice *p_latticeB, smInt p_cellIndex){
		
		for(smInt i=0;i<p_latticeA->cells[p_cellIndex].lastPrimitiveIndex;i++)
			for(smInt j=0;j<p_latticeB->cells[p_cellIndex].lastPrimitiveIndex;j++){
				if(!smAABB::checkOverlap(p_latticeA->aabb[p_latticeA->cells[p_cellIndex].cellPrimitives[i].index],
					p_latticeB->aabb[p_latticeB->cells[p_cellIndex].cellPrimitives[j].index]))
						 continue;

				 checkNarrow(p_latticeA,p_latticeB,p_latticeA->cells[p_cellIndex].cellPrimitives[i].index,p_latticeB->cells[p_cellIndex].cellPrimitives[j].index);
			}
	}

	/// \brief collision workhorse function. This is where the collision happens
	virtual void kernel(){

			collidedPairs=0;

			for(smInt i=0;i<totalLattices;i++){
			
				latticeList[i]->indexReset();
				latticeList[i]->updateBounds();
				latticeList[i]->linkPrims();
			}

			for(smInt i=0;i<totalLattices;i++){
					latticePair=latticeList[i];
					for(smInt j=i+1;j<totalLattices;j++){
						latticePair2=latticeList[j];
						for(smInt cellIndex=0;cellIndex<latticePair->totalCells;cellIndex++){

							//no primitives inside the cell
							if(latticePair2->cells[cellIndex].lastPrimitiveIndex==0||latticePair->cells[cellIndex].lastPrimitiveIndex==0)
								continue;

							 checkCells(latticePair,latticePair2,cellIndex);

						}
					}
				}
	}

	/// \brief the collision is advanced here
	virtual void run(){
		while(true||!termination){
			synch->waitTaskStart();
			kernel();
			synch->signalTaskDone();
		}
	}

	/// \brief rendering for collision visualization
	virtual void draw(){
		smInt v[3];
		smBaseMesh *baseMesh;
		smMesh *mesh;

		smGLRenderer::beginTriangles();
		glColor3fv((GLfloat*)&smColor::colorRed);
		for(smInt i=0;i<collidedPairs;i++){
			baseMesh=smSDK::getMesh(pairs[i].tri1.meshID);//correct that later on
			if(baseMesh->getType()==SOFMIS_SMMESH)
				mesh=(smMesh*)baseMesh;

			v[0]=mesh->triangles[pairs[i].tri1.primID].vert[0];
			v[1]=mesh->triangles[pairs[i].tri1.primID].vert[1];
			v[2]=mesh->triangles[pairs[i].tri1.primID].vert[2];

			smGLRenderer::drawTriangle(mesh->vertices[v[0]],mesh->vertices[v[1]],mesh->vertices[v[2]]);
		}
		smGLRenderer::endTriangles();
		glColor3fv((GLfloat*)&smColor::colorWhite);
	}

	/// \brief !!
	inline static void beginTriangles(){
		glBegin(GL_TRIANGLES);
	}

	/// \brief draws the triangle
	inline static void drawTriangle(smVec3<smFloat> &p_1,smVec3<smFloat> &p_2,smVec3<smFloat> &p_3){
		glVertex3fv((GLfloat*)&p_1);
		glVertex3fv((GLfloat*)&p_2);
		glVertex3fv((GLfloat*)&p_3);
	}

	/// \brief !!
	inline static void endTriangles(){
		glEnd();
	}

};

/// \brief 
class smSpatialGrid:public smCollisionDetection,QThread{

private:
	smSpatialGridWorker *workerThreads;
	smInt totalThreads;
	smSynchronization synch;

	smVec3<smFloat> leftCorner;
	smVec3<smFloat> rightCorner;
	smInt xSeperation;
	smInt ySeperation;
	smInt zSeperation;

	smLattice *latticeList[SOFMIS_SPATIALGRID_TOTALLATTICES];
	smInt totalLattices;
	smBool listUpdated;
	QMutex mutex;

	/// \brief !!
	void beginFrame(){
	}

	/// \brief !!
	void startWorkers(){
		for(smInt i=0;i<totalThreads;i++){
			workerThreads[i].start();
		}
	}

protected:
	smInt maxPrims;

public:

	smPipe *pipe;

	/// \brief constructor
	smSpatialGrid(smInt p_outOutputPrimSize=SOFMIS_SPATIALGRID_WORKER_COLLISIONPAIRS):synch(1){
		maxPrims=p_outOutputPrimSize;
		totalThreads=1;
		leftCorner=SOFMIS_SPATIALGRID_LEFTCORNER;
		rightCorner=SOFMIS_SPATIALGRID_RIGHTCORNER;
		xSeperation=SOFMIS_SPATIALGRID_XSEPERATION;
		ySeperation=SOFMIS_SPATIALGRID_YSEPERATION;
		zSeperation=SOFMIS_SPATIALGRID_ZSEPERATION;
		totalLattices=0;

		for(smInt i=0;i<SOFMIS_SPATIALGRID_TOTALLATTICES;i++)
			latticeList[i]=NULL;

		listUpdated=false;
		pipe=new smPipe("col_grid_tri2tri",sizeof(smCollidedTriangles),maxPrims);
	}


	/// \brief set the total number of threads for collision check
	void setTotalThreads(smInt p_totalThreads){
		if(isInitialized!=false)
			totalThreads=p_totalThreads;
		synch.setWorkerCounter(p_totalThreads);

	}

	/// \brief initialization
	void init(){
		smSceneObject *sceneObject;
		smStaticSceneObject  *staticSceneObj;
		smScene *scene;
		smClassType objectType;
		smLattice *lattices;
		smProcessID id;

		id.numbScheme=SOFMIS_PROCNUMSCHEME_X__;
		QHash<smInt, smLattice*>::iterator latticeIterator;

		if(isInitialized==true)
			return;

		workerThreads= new (smSpatialGridWorker[totalThreads]);

		for(smInt i=0;i<totalThreads;i++){
			id.x=i;
			id.totalProcX=totalThreads;
			workerThreads[i].setSynchObject(synch);
			workerThreads[i].setId(id);
		}
		updateList();
		isInitialized=true;
	}

	/// \brief run the collision checks in a loop here
	void run(){
		smCollidedTriangles *tristrisHead,*tristrisCurrent;
		smInt nbrTriTriCollisions=0;
		if(isInitialized==false)
			return;
		startWorkers();

		while(true&&this->terminateExecution==false){
			beginModule();
			nbrTriTriCollisions=0;
			synch.startTasks();
			tristrisHead=(smCollidedTriangles*)pipe->beginWrite();
			tristrisCurrent=tristrisHead;
			for(smInt i=0;i<totalThreads;i++){
				if((workerThreads[i].collidedPairs+nbrTriTriCollisions)<maxPrims){
					memcpy(tristrisCurrent,workerThreads->pairs,sizeof(smCollidedTriangles)*workerThreads[i].collidedPairs);
					tristrisCurrent+=workerThreads[i].collidedPairs;
					nbrTriTriCollisions+=workerThreads[i].collidedPairs;
				}
			}
			pipe->endWrite(nbrTriTriCollisions);
			pipe->acknowledgeValueListeners();
			endModule();
		}
		terminationCompleted=true;
	}

	/// \brief !!
	void updateList(){

		smLattice **tempLatticeList=new (smLattice*[SOFMIS_SPATIALGRID_TOTALLATTICES]);
		smInt index=0;
		mutex.lock();

		if(listUpdated==true){
			for(smInt i=0;i<SOFMIS_SPATIALGRID_TOTALLATTICES;i++)
			{		
				if(latticeList[i]!=NULL){
					tempLatticeList[index]=latticeList[i];
					index++;
				}
			}

			memcpy(&latticeList[0],&tempLatticeList[0],sizeof(smLattice*)*SOFMIS_SPATIALGRID_TOTALLATTICES);
			totalLattices=index;
			for(smInt i=0;i<totalThreads;i++){
				memcpy(&workerThreads[i].latticeList[0],tempLatticeList,sizeof(smLattice*)*SOFMIS_SPATIALGRID_TOTALLATTICES);
				workerThreads[i].totalLattices=totalLattices;
			}
		}

		listUpdated=false;
		mutex.unlock();
		delete []tempLatticeList;
	}

	/// \brief !!
	virtual void endFrame(){
		updateList();
	}

	/// \brief !!
	virtual void exec(){
		if(isInitialized)
			start();
		else{
			init();
			start();
		}
	}

	/// \brief This function adds lattice and it is  thread safe.
	virtual smInt addLattice(smLattice *p_lat){

		smSDK::addRef(p_lat);
		mutex.lock();
		latticeList[totalLattices]=p_lat;
		listUpdated=true;
		p_lat->init(SOFMIS_SPATIALGRID_LEFTCORNER,SOFMIS_SPATIALGRID_RIGHTCORNER,
			SOFMIS_SPATIALGRID_XSEPERATION,
			SOFMIS_SPATIALGRID_YSEPERATION,
			SOFMIS_SPATIALGRID_ZSEPERATION);
		totalLattices++;
		mutex.unlock();
		return totalLattices-1;
	}

	/// \brief This function removes lattice and it is  thread safe.
	virtual void removeLattice(smLattice *p_lat,smInt p_listIndex){
		smSDK::removeRef(p_lat);
		mutex.lock();
		latticeList[p_listIndex]=NULL;
		totalLattices--;
		listUpdated=true;
		mutex.unlock();
	}

	/// \brief !! renders the workers threads
	void draw(smDrawParam p_params){
		for(smInt i=0;i<totalThreads;i++){
			workerThreads[i].draw();
		}
	}

};

#endif
