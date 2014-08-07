//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "gui/buttons/guiIconButtonCtrl.h"
#include "gui/editor/guiInspector.h"
#include "gui/editor/inspector/prefabGroup.h"
//#include "gui/editor/inspector/ComponentField.h"
#include "core/strings/stringUnit.h"
//#include "T3D/Entity.h"
#include "component/components/component.h"
#include "T3D/prefab.h"
#include "gui/editor/guiInspectorTypes.h"

IMPLEMENT_CONOBJECT(GuiInspectorPrefabGroup);

ConsoleDocClass( GuiInspectorPrefabGroup,
   "@brief Used to inspect an object's FieldDictionary (dynamic fields) instead "
   "of regular persistent fields.\n\n"
   "Editor use only.\n\n"
   "@internal"
);

//-----------------------------------------------------------------------------
// GuiInspectorPrefabGroup - add custom controls
//-----------------------------------------------------------------------------
bool GuiInspectorPrefabGroup::createContent()
{
   if(!Parent::createContent())
      return false;

   //give the necessary padding for the nested controls so it looks nice.
   setMargin(RectI(4,0,4,4));

   //Con::evaluatef( "%d.stack = %d;", this->getId(), mStack->getId() );

   //Con::executef( this, "createContent" );

   // encapsulate the button in a dummy control.
   /*GuiControl* shell = new GuiControl();
   shell->setDataField( StringTable->insert("profile"), NULL, "GuiTransparentProfile" );
   if( !shell->registerObject() )
   {
      delete shell;
      return false;
   }

   // add a button that lets us add new dynamic fields.
   GuiBitmapButtonCtrl* addBehaviorBtn = new GuiBitmapButtonCtrl();
   {
      SimObject* profilePtr = Sim::findObject("InspectorDynamicFieldButton");
      if( profilePtr != NULL )
         addBehaviorBtn->setControlProfile( dynamic_cast<GuiControlProfile*>(profilePtr) );
		
		// FIXME Hardcoded image
      addBehaviorBtn->setBitmap("tools/gui/images/iconAdd.png");

      //char commandBuf[64];
      //dSprintf(commandBuf, 64, "%d.addComponent();", this->getId());
      //addBehaviorBtn->setField("command", commandBuf);
      addBehaviorBtn->setSizing(horizResizeRight,vertResizeCenter);
      //addFieldBtn->setField("buttonMargin", "2 2");
      addBehaviorBtn->resize(Point2I(0,2), Point2I(16, 16));
      addBehaviorBtn->registerObject("AddBehaviorButton");
   }

   mAddBhvrList = new GuiPopUpMenuCtrlEx();
   {
      SimObject* profilePtr = Sim::findObject("GuiPopUpMenuProfile");
      if( profilePtr != NULL )
         mAddBhvrList->setControlProfile( dynamic_cast<GuiControlProfile*>(profilePtr) );

	  // Configure it to update our value when the popup is closed
	  //char szBuffer[512];
	  //dSprintf( szBuffer, 512, "%d.apply( %d.getText() );", getId(), addBehaviorList->getId() );
	  //addBehaviorList->setField("Command", szBuffer );

	  //now add the entries, allow derived classes to override this
	  //_populateMenu( addBehaviorList );

	  // Select the active item, or just set the text field if that fails
	  //S32 id = addBehaviorList->findText(getData());
	  //if (id != -1)
	  //  addBehaviorList->setSelected(id, false);
	  //else
	  //  addBehaviorList->setField("text", getData());

	  mAddBhvrList->setSizing(horizResizeWidth,vertResizeCenter);
	  mAddBhvrList->resize(Point2I(addBehaviorBtn->getPosition().x + addBehaviorBtn->getExtent().x + 2 ,2), Point2I(getWidth() - 16, 16));
      mAddBhvrList->registerObject("eBehaviorList");
   }

   shell->resize(Point2I(0,0), Point2I(getWidth(), 28));
   shell->addObject(addBehaviorBtn);
   shell->addObject(mAddBhvrList);

   // save off the shell control, so we can push it to the bottom of the stack in inspectGroup()
   mAddCtrl = shell;
   mStack->addObject(shell);*/

	
	Prefab* target = dynamic_cast<Prefab*>(mParent->getInspectObject(0));
	bool targetDirty = target->isDirty();

	char cmdBuffer[512];

	reloadFile = new GuiButtonCtrl();
	reloadFile->registerObject();
	reloadFile->setExtent(getExtent().x, 25);
	reloadFile->setText("Reload from file");
	reloadFile->setDataField( StringTable->insert("Profile"), NULL, "GuiInspectorButtonProfile" );

	if(!targetDirty)
		reloadFile->setActive(false);
	
   dSprintf( cmdBuffer, 512, "%d.reloadFromFile();", target->getId() );

   reloadFile->setField( "Command", cmdBuffer );

	saveFile = new GuiButtonCtrl();
	saveFile->registerObject();
	saveFile->setExtent(getExtent().x, 25);
	saveFile->setText("Save to new prefab");
	saveFile->setDataField( StringTable->insert("Profile"), NULL, "GuiInspectorButtonProfile" );

	dSprintf( cmdBuffer, 512, "EditorMakeNewPrefab();" );

   saveFile->setField( "Command", cmdBuffer );

	overwriteFile = new GuiButtonCtrl();
	overwriteFile->registerObject();
	overwriteFile->setExtent(getExtent().x, 25);
	overwriteFile->setText("Overwrite prefab");
	overwriteFile->setDataField( StringTable->insert("Profile"), NULL, "GuiInspectorButtonProfile" );

	if(!targetDirty)
		overwriteFile->setActive(false);

	dSprintf( cmdBuffer, 512, "EditorOverwritePrefab();" );

   overwriteFile->setField( "Command", cmdBuffer );

	//
	filePath = new GuiControl();
	filePath->registerObject();
	filePath->setExtent(getExtent().x, 25);

	GuiTextEditCtrl* retCtrl = new GuiTextEditCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   // Let's make it look pretty.
   retCtrl->setDataField( StringTable->insert("profile"), NULL, "GuiInspectorTextEditRightProfile" );
   retCtrl->setDataField( StringTable->insert("tooltipprofile"), NULL, "GuiToolTipProfile" );
   retCtrl->setDataField( StringTable->insert("hovertime"), NULL, "1000" );

   // Don't forget to register ourselves
	retCtrl->registerObject();
	filePath->addObject(retCtrl);

   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.filename = %d.getText();",target->getId(),retCtrl->getId() );
   retCtrl->setField("AltCommand", szBuffer );
   retCtrl->setField("Validate", szBuffer );
	retCtrl->setText(target->getFilename());

   mBrowseButton = new GuiButtonCtrl();

   if( mBrowseButton != NULL )
   {
      RectI browseRect( Point2I( ( filePath->getLeft() + filePath->getWidth()) - 26, filePath->getTop() + 2), Point2I(20, filePath->getHeight() - 4) );
      const char *fileSpec = "Prefab Files (*.prefab)|*.prefab|All Files (*.*)|*.*|";

      dSprintf( cmdBuffer, 512, "getLoadFilename(\"%s\", \"%d.apply\", %d.getData());", fileSpec, getId(), getId() );

      mBrowseButton->setField( "Command", cmdBuffer );
      mBrowseButton->setField( "text", "..." );
      mBrowseButton->setDataField( StringTable->insert("Profile"), NULL, "GuiInspectorButtonProfile" );
      mBrowseButton->registerObject();
      //addObject( mBrowseButton );
		filePath->addObject(mBrowseButton);

      // Position
      mBrowseButton->resize( browseRect.point, browseRect.extent );
   }

	//filePath->addObject(mBrowseButton);
	//filePath->addObject(retCtrl);
	//
	
   mStack->addObject(reloadFile);
	mStack->addObject(saveFile);
	mStack->addObject(overwriteFile);	
	mStack->addObject(filePath);

   return true;
}

//-----------------------------------------------------------------------------
// GuiInspectorPrefabGroup - inspectGroup override
//-----------------------------------------------------------------------------
bool GuiInspectorPrefabGroup::inspectGroup()
{
   // clear the first responder if it's set
   //mStack->clearFirstResponder();

   // Clearing the fields and recreating them will more than likely be more
   // efficient than looking up existent fields, updating them, and then iterating
   // over existent fields and making sure they still exist, if not, deleting them.
   //clearFields();

   //mStack->pushObject(mAddCtrl);

   const U32 numTargets = mParent->getNumInspectObjects();
   if(numTargets == 1)
   {
	   //ComponentObject* target = dynamic_cast<ComponentObject*>(mParent->getInspectObject(0));

	   //Con::executef( this, "inspectObject", target->getIdString() );

	   //Lets update our add behavior functionality to properly reference the target
       /*Con::evaluatef( "%d.objectToAdd = %d;", mAddBhvrList->getId(), target->getId() );

	   //get all our behaviors, if any
	   for( U32 i = 0; i < target->getComponentCount(); i++)
	   {
		    if(target->getComponent(i)->isHidden())
				continue;

		    StringTableEntry templateName = StringTable->insert(target->getComponent(i)->getTemplateName());
			GuiRolloutCtrl *behaviorCtrl = new GuiRolloutCtrl();
			char szName[512];
			//dSprintf( szName, 512, "IE_%d_%s_Behavior", behaviorCtrl->getClassName(), target->getId(), templateName );
			dSprintf( szName, 512, "IE_%d_%s_Behavior", target->getId(), templateName );

			if(behaviorCtrl->registerObject(szName))
		    {
			   behaviorCtrl->setDataField( StringTable->insert("profile"), NULL, "GuiRolloutProfile" );
			   behaviorCtrl->setSizing(horizResizeWidth,vertResizeCenter);
			   behaviorCtrl->setCaption(templateName);
			   mStack->addObject(behaviorCtrl);

			   behaviorCtrl->resize(Point2I(0,2), getExtent());
			   //give the necessary padding for the nested controls so it looks nice.
			   behaviorCtrl->setMargin(RectI(3,3,3,3));

			   //first build our behaviors stack
			   GuiStackControl* behaviorFieldStack = new GuiStackControl();
			   //dSprintf( szName, 512, "IE_%s_%d_%s_bhvStk", behaviorCtrl->getClassName(), target->getId(), templateName );
			   if(behaviorFieldStack->registerObject())
			   {
				    behaviorFieldStack->resize(Point2I(0,0), behaviorCtrl->getExtent());
					behaviorFieldStack->setPadding(4);
					behaviorCtrl->addObject(behaviorFieldStack);

					ComponentInstance* bI = target->getComponent(i);
					if(bI != NULL)
					{
						for(U32 bF = 0; bF < bI->getTemplate()->getComponentFieldCount(); bF++)
						{
							GuiInspectorBehaviorField *bhvFld = new GuiInspectorBehaviorField( mParent, this, bI, bF );

							// Register the inspector field and add it to our lists
							if( bhvFld->registerObject() )
							{
								//mChildren.push_back( bhvFld );
								behaviorFieldStack->addObject( bhvFld );
							}
							else
								delete bhvFld;
						}
					}

					GuiIconButtonCtrl *deleteButton = new GuiIconButtonCtrl();
					dSprintf( szName, 512, "IE_%s_%d_%s_bhvDel", behaviorCtrl->getClassName(), target->getId(), templateName );
				    if(deleteButton->registerObject())
				    {
					   SimObject* profilePtr = Sim::findObject("ToolsGuiButtonProfile");
					   if( profilePtr != NULL )
						 deleteButton->setControlProfile( dynamic_cast<GuiControlProfile*>(profilePtr) );
						
					   // FIXME Hardcoded image
					   deleteButton->setBitmap("tools/gui/images/iconDelete.png");
					   deleteButton->setTextLocation(GuiIconButtonCtrl::TextLocCenter);
					   deleteButton->setText("Remove This Behavior");

					   char buf[64];
					   //dSprintf(commandBuf, 64, "%d.removeBehavior(%d,true); %d.refesh();", target->getId(),bI->getId(),mParent->getId());
					   //deleteButton->setField("command", commandBuf);

					   deleteButton->setSizing(horizResizeWidth,vertResizeCenter);
					   deleteButton->setExtent(Point2I(behaviorFieldStack->getExtent().x, 16));
					   deleteButton->setClassNamespace("RemoveBehaviorButton");
					   Con::evaluatef( "%d.object = %d;", deleteButton->getId(), target->getId(), buf );
					   Con::evaluatef( "%d.behavior = %d;", deleteButton->getId(), bI->getId(), buf );

					   behaviorFieldStack->addObject(deleteButton);
				    }
				    else
					   delete deleteButton;

					behaviorFieldStack->updatePanes();
			   }
			   else
				   delete behaviorFieldStack;
		    }
			else
				delete behaviorCtrl;
	   }*/
   }

   //mStack->updatePanes();
   
   //setUpdate();

   return true;
}

void GuiInspectorPrefabGroup::updateAllFields()
{
   // We overload this to just reinspect the group.
   inspectGroup();
}

void GuiInspectorPrefabGroup::onMouseMove(const GuiEvent &event)
{
	//mParent->mOverDivider = false;
}
ConsoleMethod(GuiInspectorPrefabGroup, inspectGroup, bool, 2, 2, "Refreshes the dynamic fields in the inspector.")
{
   return object->inspectGroup();
}

void GuiInspectorPrefabGroup::clearFields()
{
   // save mAddCtrl
   //Sim::getGuiGroup()->addObject(mAddCtrl);
   // delete everything else
   //mStack->clear();
   // clear the mChildren list.
   //mChildren.clear();
   // and restore.
   //mStack->addObject(mAddCtrl);
}

SimFieldDictionary::Entry* GuiInspectorPrefabGroup::findDynamicFieldInDictionary( StringTableEntry fieldName )
{
   SimFieldDictionary * fieldDictionary = mParent->getInspectObject()->getFieldDictionary();

   for(SimFieldDictionaryIterator ditr(fieldDictionary); *ditr; ++ditr)
   {
      SimFieldDictionary::Entry * entry = (*ditr);

      if( entry->slotName == fieldName )
         return entry;
   }

   return NULL;
}

void GuiInspectorPrefabGroup::addDynamicField()
{
   // We can't add a field without a target
   /*if( !mStack )
   {
      Con::warnf("GuiInspectorPrefabGroup::addDynamicField - no target SimObject to add a dynamic field to.");
      return;
   }

   // find a field name that is not in use. 
   // But we wont try more than 100 times to find an available field.
   U32 uid = 1;
   char buf[64] = "dynamicField";
   SimFieldDictionary::Entry* entry = findDynamicFieldInDictionary(buf);
   while(entry != NULL && uid < 100)
   {
      dSprintf(buf, sizeof(buf), "dynamicField%03d", uid++);
      entry = findDynamicFieldInDictionary(buf);
   }
   
   const U32 numTargets = mParent->getNumInspectObjects();
   if( numTargets > 1 )
      Con::executef( mParent, "onBeginCompoundEdit" );

   for( U32 i = 0; i < numTargets; ++ i )
   {
      SimObject* target = mParent->getInspectObject( i );
      
      Con::evaluatef( "%d.dynamicField = \"defaultValue\";", target->getId(), buf );
 
      // Notify script.
   
      Con::executef( mParent, "onFieldAdded", target->getIdString(), buf );
   }
   
   if( numTargets > 1 )
      Con::executef( mParent, "onEndCompoundEdit" );

   // now we simply re-inspect the object, to see the new field.
   inspectGroup();
   instantExpand();*/
}

AbstractClassRep::Field* GuiInspectorPrefabGroup::findObjectBehaviorField(ComponentInstance* target, String fieldName)
{
   AbstractClassRep::FieldList& fieldList = target->getClassRep()->mFieldList;
   for( AbstractClassRep::FieldList::iterator itr = fieldList.begin();
		itr != fieldList.end(); ++ itr )
   {
	  AbstractClassRep::Field* field = &( *itr );
	  String fldNm(field->pFieldname);
	  if(fldNm == fieldName)
		  return field;
   }
   return NULL;
}
ConsoleMethod( GuiInspectorPrefabGroup, addDynamicField, void, 2, 2, "obj.addDynamicField();" )
{
   object->addDynamicField();
}

ConsoleMethod( GuiInspectorPrefabGroup, removeDynamicField, void, 3, 3, "" )
{
}
