/*
****************************************************
                  SOFMIS LICENSE

****************************************************

    \author:    <http:\\acor.rpi.edu>
                SOFMIS TEAM IN ALPHABATIC ORDER
                Anderson Maciel, Ph.D.
                Ganesh Sankaranarayanan, Ph.D.
                Sreekanth A Venkata
                Suvranu De, Ph.D.
                Tansel Halic
                Zhonghua Lu

    \author:    Module by Tansel Halic
                
                
    \version    1.0
    \date       05/2009
    \bug	    None yet
    \brief	    This class is the simulator object. Each simulator should derive this.
                
    

*****************************************************
*/

#ifndef SMDUMMYSIMULATOR_H
#define SMDUMMYSIMULATOR_H
#include "smCore/smConfig.h"
#include "smCore/smObjectSimulator.h"
#include "smCore/smErrorLog.h"
#include "smCore/smEventHandler.h"
#include "smCore/smEventData.h"


///example simulator..the dummy simulator works on static scene objects for now..
class smDummySimulator:public smObjectSimulator,public smEventHandler{

private:
	  

public:
	smDummySimulator(smErrorLog *p_errorLog):smObjectSimulator(p_errorLog){
		

	
	
	}


protected:
	virtual void beginSim(){
		//start the job
	}


	virtual void initCustom(){
		smClassType type;
		smSceneObject *object;
		smStaticSceneObject *staticObject;
		smVec3<smFloat> *newVertices;
		//do nothing for now
		for(smInt i=0;i<objectsSimulated.size();i++){
			object=objectsSimulated[i];
			type=object->getType();
			switch(type){ 
				case SOFMIS_SMSTATICSCENEOBJECT:
					staticObject=(smStaticSceneObject*)object;
					object->memBlock->allocate<smVec3<smFloat>>(QString("pos"),staticObject->mesh->nbrVertices);
					object->memBlock->originaltoLocalBlock(QString("pos"),staticObject->mesh->vertices,staticObject->mesh->nbrVertices);
					object->flags.isSimulatorInit=true;
					
				break;
			}
			
		
		}

	}


	//test
	virtual void run(){
		smSceneObject *sceneObj;
		smStaticSceneObject *staticSceneObject;
		smVec3<smFloat> *vertices;
		smMesh *mesh;
		
		beginSim();
		for(smInt i=0;i<this->objectsSimulated.size();i++){
			  sceneObj=this->objectsSimulated[i];

			  //ensure that dummy simulator will work on static scene objects only.
			  if(sceneObj->getType()==SOFMIS_SMSTATICSCENEOBJECT){
				  staticSceneObject=(smStaticSceneObject*)sceneObj;
				  mesh=staticSceneObject->mesh;
				  
				  staticSceneObject->memBlock->getBlock(QString("pos"),(void**)&vertices);
				  for(smInt vertIndex=0;vertIndex<staticSceneObject->mesh->nbrVertices;vertIndex++){
					  vertices[vertIndex].y=vertices[vertIndex].y+0.000001;
				  
				  
				  }
			  
			  
			  }

			
			
		  }
		  //wait
		  //do the job
		  //write the result
		 endSim();
		
			

	
	
	}

	void endSim(){
		//end the job
	
	}

	///synchronize the buffers in the object..do not call by yourself.
	void syncBuffers(){
		smSceneObject *sceneObj;
		smStaticSceneObject *staticSceneObject;
		smVec3<smFloat> *vertices;
		smMesh *mesh;

		for(smInt i=0;i<this->objectsSimulated.size();i++){
			  sceneObj=this->objectsSimulated[i];
			  //ensure that dummy simulator will work on static scene objects only.
			  if(sceneObj->getType()==SOFMIS_SMSTATICSCENEOBJECT){
				  staticSceneObject=(smStaticSceneObject*)sceneObj;
				  mesh=staticSceneObject->mesh;
				  staticSceneObject->memBlock->localtoOriginalBlock(QString("pos"),mesh->vertices,mesh->nbrVertices);
				  
			  
			  }

			
			
		}

	  
	}

	void handleEvent(smEvent *p_event){
		smKeyboardEventData *keyBoardData;
				
		switch(p_event->eventType.eventTypeCode){

		case SOFMIS_EVENTTYPE_KEYBOARD:
			keyBoardData=(smKeyboardEventData*)p_event->data;
			if(keyBoardData->keyBoardKey==Qt::Key_F1)
				printf("F1 Keyboard is pressed %c\n",keyBoardData->keyBoardKey);
			break;

		
		
		}
	
	}

};





#endif