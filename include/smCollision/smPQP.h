#ifndef SMPQP_H
#define SMPQP_H

#include "smCore/smConfig.h"
#include "smCollision/smCollisionConfig.h"
#include "smCore/smObjectSimulator.h"
#include "smCore/smErrorLog.h"
#include "smCore/smEventHandler.h"
#include "smCore/smEventData.h"
#include "smMesh/smMesh.h"
#include "smCore/smDataStructs.h"
#include "smCore/smPipe.h"
#include <Qvector>
#include "PQP.h"
#include "smRendering/smGLRenderer.h"
class smPipe;
 
struct smPQPResult{
	smVec3f point1;
	smVec3f point2;
	smFloat distance;

};

class smPQPSkeleton:public smCoreClass{
public:
	smMatrix33f mR;
	smVec3f mT;
	smMatrix44f mat;

	PQP_Model  *mPQPModel;
	smSurfaceMesh *mMesh;
	PQP_DistanceResult colRes;
	smUnifiedID colMeshId;///stores the closest mesh id
	smPQPSkeleton *colSkel;
	inline void setTransformation(smMatrix44d &p_trans){
		mat=p_trans;
		mR=p_trans;
		mT=p_trans;
	
	
	}
	
	smPQPSkeleton(smSurfaceMesh *p_mesh){
		mT.setValue(0,0,0);
		mR.setIdentity();

		mMesh=p_mesh;
		mPQPModel=new PQP_Model();
		mPQPModel->BeginModel();
		for(int i=0;i<mMesh->nbrTriangles;i++)
			mPQPModel->AddTri((const PQP_REAL*)(&mMesh->vertices[mMesh->triangles[i].vert[0]]),
			(const PQP_REAL*)&mMesh->vertices[mMesh->triangles[i].vert[1]],
			(const PQP_REAL*)&mMesh->vertices[mMesh->triangles[i].vert[2]],i);


		mPQPModel->EndModel();

	}

};


///PQP based collision detection
class smPQPCollision:public smObjectSimulator,public smEventHandler{
public:

	smMatrix44d mat;

	smVec3d pos;
	smFloat minCollisionDistance;///The default value is 1.0
	smBool  minCollisionHappened;
	smUnifiedID   onlySpecificMeshId;//collision check is done only with a specific mesh if this is given. This will have the mesh id;
	
	smPipe *pipePQP;


	smPQPCollision(  smErrorLog *p_errorLog=NULL,smInt maxColResult=500):smObjectSimulator(p_errorLog) 
	{
		minCollisionDistance=1.5;//default distance
		//minCollisionDistance=5.0;
		onlySpecificMeshId.ID=-1;
		pipePQP=new smPipe("col_PQP",sizeof(smPQPResult),maxColResult);
								
	}
	void initCustom(){
	}
	void draw(smDrawParam p_params){
		double matrixArray[16];
		smInt nbrSrcMeshes=mPQPSourceSkeletons.size();
		glPushMatrix();
		for(smInt i=0;i<nbrSrcMeshes;i++){
			mat.getMatrixForOpenGL(matrixArray);
			glMultMatrixd(matrixArray);
			smGLRenderer::drawSurfaceMeshTriangles(mPQPSourceSkeletons[i]->mMesh,&p_params.caller->renderDetail,p_params);
		
		
		}
		glPopMatrix();
		//checkCollision();
	}
	void initDraw(smDrawParam p_param){
	}
	void syncBuffers(){





	}
	virtual void run(){
	}
	vector<smPQPSkeleton *>mPQPSourceSkeletons;
	vector<smPQPSkeleton *>mPQPDestinationSkeletons;

	void addDestinationMeshes(smSurfaceMesh *p_mesh){
		smPQPSkeleton *pqpSkeleton=new smPQPSkeleton(p_mesh);
		mPQPDestinationSkeletons.push_back(pqpSkeleton);

	}
	smPQPSkeleton* addSourceMeshes(smSurfaceMesh *p_mesh){
		smPQPSkeleton *pqpSkeleton=new smPQPSkeleton(p_mesh);
		mPQPSourceSkeletons.push_back(pqpSkeleton);
		return pqpSkeleton;


	}
	void checkCollision(){
		smInt nbrDestMeshes=mPQPDestinationSkeletons.size();
		smInt nbrSrcMeshes=mPQPSourceSkeletons.size();
		smPQPSkeleton *src;
		smPQPSkeleton *dest;
		PQP_DistanceResult colRes;
		PQP_REAL rel_err = 0.00000;
		PQP_REAL abs_err = 0.00000;
		smPQPResult *collisionResult;
		smInt t=0;

		minCollisionHappened=false;
		
		
		 

		collisionResult=(smPQPResult*)pipePQP->beginWrite();
		for(smInt i=0;i<nbrSrcMeshes;i++){
			src=mPQPSourceSkeletons[i];
			src->colRes.distance=1e15;
			src->colSkel=NULL;


			for(smInt j=0;j<nbrDestMeshes;j++){
				
				dest=mPQPDestinationSkeletons[j];
				if(onlySpecificMeshId.ID>0&&!(onlySpecificMeshId==dest->mMesh->uniqueId))
					continue;
				
				

				PQP_Distance(&colRes,src->mR.e,(float*)&src->mT, src->mPQPModel,dest->mR.e,(float*)&dest->mT,dest->mPQPModel,rel_err,abs_err);
				if(colRes.Distance()<src->colRes.distance){
						  
						   src->colRes=colRes;
						   src->colMeshId=mPQPDestinationSkeletons[j]->mMesh->uniqueId;
						   src->colSkel=mPQPDestinationSkeletons[j];
						  // cout<<"[checkCol]:"<<onlySpecificMeshId.ID<<endl;
				}
				if(colRes.Distance()<minCollisionDistance){
					
					
					minCollisionHappened=true;
					smVec3f p1;
					smVec3f p2;
					//cout<<"Collisions in Draw"<<endl;
					p1.setValue(colRes.p1[0],colRes.p1[1],colRes.p1[2]);
					p2.setValue(colRes.p2[0],colRes.p2[1],colRes.p2[2]);
					if(t<pipePQP->getElements()){
						collisionResult[t].point1=p1;
						collisionResult[t].point2=p2;
						collisionResult[t].distance=colRes.Distance();
						t++;
					}
					glPushMatrix();
						
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, smColor::colorYellow.toGLColor());
						glTranslatef(p1.x,p1.y,p1.z);
						glutSolidSphere(2.0,15,15);
					glPopMatrix();

					glPushMatrix();
		
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, smColor::colorPink.toGLColor());
						//glTranslatef(T2.x,T2.y,T2.z);
						glTranslatef(p2.x,p2.y,p2.z);
						glutSolidSphere(2.0,15,15);
					glPopMatrix();

				}

			}



		}
		pipePQP->endWrite(t);
		pipePQP->acknowledgeValueListeners();

	}

	 void smPQPCollision::handleEvent(smEvent *p_event)
	{
		smHapticOutEventData *hapticEventData;
		smVec3d rightVector;
		switch(p_event->eventType.eventTypeCode){
			case SOFMIS_EVENTTYPE_HAPTICOUT:
				

				hapticEventData=(smHapticOutEventData *)p_event->data;
				if(hapticEventData->deviceId==0){			
					
					//pos=hapticEventData->position;
					//mat=hapticEventData->transform;
					//pos=hapticEventData->transform;
					//smInt nbrSrcMeshes=mPQPSourceSkeletons.size();
					//for(smInt i=0;i<nbrSrcMeshes;i++)
						//mPQPSourceSkeletons[i]->setTransformation(hapticEventData->transform);
					
				
					
					
				

				

				}
				break;
			

		}
	}






};




#endif 

