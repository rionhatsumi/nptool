/*****************************************************************************
 * Copyright (C) 2009-2016   this file is part of the NPTool Project         *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: Adrien Matta   contact address: matta@lpccaen.in2p3.fr   *
 *                                                                           *
 * Creation Date  :                                                          *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *                                                                           *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include"NPSpectraClient.h" 
#include<iostream>
#include<sstream>
#include"NPCore.h"
////////////////////////////////////////////////////////////////////////////////
NPL::SpectraClient::SpectraClient(){
  m_Sock =NULL;
  m_Spectra=NULL; 
  m_Delta = NULL;
  m_Address = "localhost";
  m_Port = 9092;

}
////////////////////////////////////////////////////////////////////////////////
NPL::SpectraClient::SpectraClient(std::string address, int port){
  m_Sock =NULL;
  m_Spectra=NULL; 
  m_Delta = NULL;
  m_Address = address;
  m_Port = port;
  Connect();
}
////////////////////////////////////////////////////////////////////////////////
NPL::SpectraClient::~SpectraClient(){
  if(m_Sock){
    m_Sock->Close("force");
    delete m_Sock;
    m_Sock = NULL;
  } 
  if(m_Spectra){
    m_Spectra->Delete();
    delete m_Spectra;
    m_Spectra=NULL;
  }
}
////////////////////////////////////////////////////////////////////////////////
bool NPL::SpectraClient::Connect(){
  // Connect to SpectraServer
  if(m_Sock){
    m_Sock->Close("force");
    delete m_Sock;
    m_Sock = NULL;
  }
  m_Sock = new TSocket(m_Address.c_str(),m_Port);
  if(m_Sock->IsValid()){
    //Sync();
    return true;
  }
  else{
    std::ostringstream message;
    message << "Connection to " << m_Address << ":" << m_Port << " Failed";
    NPL::SendWarning("NPL::SpectraClient",message.str());

    return false;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool NPL::SpectraClient::Sync(){
  if(!m_Sock || !(m_Sock->IsValid())){
    if(m_Sock){
      m_Sock->Close("force");
      delete m_Sock;
      m_Sock = NULL;
    }
    NPL::SendWarning("NPL::SpectraClient","Sync failed: Connection lost");

    return false;
  }

  TMessage* message=NULL;
  m_Sock->Send("RequestSpectra");

  if(m_Sock->Recv(message)<=0){
    if(m_Sock){
      m_Sock->Close("force");
      delete m_Sock;
      m_Sock = NULL;
    }
    NPL::SendWarning("NPL::SpectraClient","Sync failed: message return unreadable");

    return false ;
  }

  if(message){
    if(m_Spectra){
      m_Spectra->Delete();
      delete m_Spectra;
      m_Spectra = NULL;
    }
    m_Spectra = (TList*) message->ReadObject(message->GetClass());
    return true;
  }

  else{
    NPL::SendInformation("NPL::SpectraClient","Server return empty sepctra list");
    return false;
  }

}

////////////////////////////////////////////////////////////////////////////////
bool NPL::SpectraClient::Update(){
  if(!m_Sock || !(m_Sock->IsValid())){
    if(m_Sock){
      m_Sock->Close("force");
      delete m_Sock;
      m_Sock = NULL;
    }
    NPL::SendWarning("NPL::SpectraClient","Update failed: Connection lost");

    return false;
  }

  TMessage* message=NULL;
  m_Sock->Send("RequestDelta");

  if(m_Sock->Recv(message)<=0){
    if(m_Sock){
      m_Sock->Close("force");
      delete m_Sock;
      m_Sock = NULL;
    }

    NPL::SendWarning("NPL::SpectraClient","Update failed: message return unreadable");

    return false;
  }

  if(message){
    m_Delta = (TList*) message->ReadObject(message->GetClass());
    // Loop over delta to apply update
    //    for(void* c = (void*) m_Delta->First() ; c !=0 ; c = (void*) m_Delta->After(c)){
    // Fill some stuff
    //  }
    return true;
  }
  else{
    NPL::SendInformation("NPL::SpectraClient","Server return empty update list");
    return true;
  }
}

////////////////////////////////////////////////////////////////////////////////
TList* NPL::SpectraClient::GetSpectra(){
  return m_Spectra;
}

