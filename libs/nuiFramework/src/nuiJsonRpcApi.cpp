/**
 * \file      nuiJsonRpcApi.cpp
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \author    Scott Halstvedt
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */

#include "nuiJsonRpcApi.h"
#include "nuiFrameworkManager.h"

LOG_DECLARE("RPC");

//! \def ADDRPCMETHOD(FUNC, RPCCALL)
//! Shortcut to easier add/control methods of JsonRpcApi
#define ADDRPCMETHOD(func) server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::##func##, std::string(#func)));

nuiJsonRpcApi *nuiJsonRpcApi::getInstance()
{
	static nuiJsonRpcApi *instance = NULL;
	if (instance == NULL)
		instance = new nuiJsonRpcApi();
	return instance;
};

template<class T>
std::string t_to_string(T i)
{
	std::stringstream ss;
	std::string s;
	ss << i;
	s = ss.str();

	return s;
}

bool nuiJsonRpcApi::init(std::string address, int port)
{

#ifdef WIN32
	// initialize network for Win32 platform
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) == -1) {
			LOG(NUI_CRITICAL, "unable to initialize WinSock (v2.2)");
			return false;
		}
	}
#endif

	if (server == NULL)
		server = new Json::Rpc::TcpServer(address, port);

	if (!server->Bind())
	{
		delete server;
		LOG(NUI_CRITICAL, "unable to bind the JSON RPC daemon");
		return false;
	}
	if (!server->Listen())
	{
		delete server;
		LOG(NUI_CRITICAL, "unable to open the socket for listening");
		return false;
	}

	this->finished = false;
	LOG(NUI_DEBUG, "running");

	ADDRPCMETHOD(nui_list_dynamic);
	ADDRPCMETHOD(nui_list_pipelines);
	ADDRPCMETHOD(nui_workflow_start);
	ADDRPCMETHOD(nui_workflow_stop);
	ADDRPCMETHOD(nui_workflow_quit);
	ADDRPCMETHOD(nui_create_pipeline);
	ADDRPCMETHOD(nui_create_module);
	ADDRPCMETHOD(nui_create_connection);
	ADDRPCMETHOD(nui_update_pipeline);
	ADDRPCMETHOD(nui_update_pipelineProperty);
	ADDRPCMETHOD(nui_update_moduleProperty);
	ADDRPCMETHOD(nui_update_endpoint);
	ADDRPCMETHOD(nui_update_connection);
	ADDRPCMETHOD(nui_update_endpointCount);
	ADDRPCMETHOD(nui_delete_pipeline);
	ADDRPCMETHOD(nui_delete_module);
	ADDRPCMETHOD(nui_delete_endpoint);
	ADDRPCMETHOD(nui_delete_connection);
	ADDRPCMETHOD(nui_get_current);
	ADDRPCMETHOD(nui_get_pipeline);
	ADDRPCMETHOD(nui_get_module);
	ADDRPCMETHOD(nui_get_connection);
	ADDRPCMETHOD(nui_navigate_push);
	ADDRPCMETHOD(nui_navigate_pop);
	ADDRPCMETHOD(nui_save_workflow);
	ADDRPCMETHOD(nui_get_module_properties);
	ADDRPCMETHOD(nui_save_configuration);
	ADDRPCMETHOD(nui_get_availible_modules);
	ADDRPCMETHOD(nui_commit_pipeline);

	return true;
};

bool nuiJsonRpcApi::isInitialized()
{
	if (server != NULL)
		return true;
	return false;
};

void nuiJsonRpcApi::startApi()
{
	if (isInitialized())
		start();
};

void nuiJsonRpcApi::stopApi(bool killServer)
{
	this->want_quit = true;
};

bool nuiJsonRpcApi::isFinished()
{
	return finished;
};

void nuiJsonRpcApi::execute()
{
	do
	{
		server->WaitMessage(1000);
	} while (!want_quit);
};

void nuiJsonRpcApi::cleanup() {
	if (this->server != NULL)
		server->Close();
	delete server;
#ifdef _WIN32
	WSACleanup();
#endif
	finished = true;
};

nuiJsonRpcApi::nuiJsonRpcApi() : pt::thread(false)
{
	this->want_quit = false;
	this->server = NULL;
};

bool nuiJsonRpcApi::nui_list_dynamic(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::vector<std::string>* list = nuiFrameworkManager::getInstance().listModules();

	Json::Value* jModules = new Json::Value();
	std::vector<std::string>::iterator it;
	for (it = list->begin(); it != list->end(); it++)
		jModules->append(*it);

	setSuccess(response);
	response["result"] = *jModules;
	response["data_type"] = "list";

	return true;
}

bool nuiJsonRpcApi::nui_list_pipelines(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string hosterName = root["params"]["hostername"].asString();

	std::vector<std::string> *list;
	//uncomment
	  //list = nuiFrameworkManager::getInstance().listPipelines(hosterName);

	Json::Value* jModules = new Json::Value();
	std::vector<std::string>::iterator it;
	for (it = list->begin(); it != list->end(); it++)
		jModules->append(*it);

	setSuccess(response);
	response["result"] = *jModules;
	response["data_type"] = "list";

	return true;
}

bool nuiJsonRpcApi::nui_workflow_start(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	nuiFrameworkManagerErrorCode::err error = nuiFrameworkManager::getInstance().workflowStart();
	if (error == nuiFrameworkManagerErrorCode::Success)
	{
		setSuccess(response);
		return true;
	}
	else
	{
		setFailure(response);
		return false;
	}
}

bool nuiJsonRpcApi::nui_workflow_stop(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	nuiFrameworkManagerErrorCode::err error = nuiFrameworkManager::getInstance().workflowStop();
	if (error == nuiFrameworkManagerErrorCode::Success)
	{
		setSuccess(response);
		return true;
	}
	else
	{
		setFailure(response);
		return false;
	}
}

bool nuiJsonRpcApi::nui_workflow_quit(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	this->stopApi();
	setSuccess(response);
	return true;
}

bool nuiJsonRpcApi::nui_create_pipeline(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();

	nuiFrameworkManager& manager = nuiFrameworkManager::getInstance();

	nuiModuleDescriptor* descriptor = manager.createNewPipelineTemplate(pipeline);

	if (descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_pipeline(descriptor);
		response["data_type"] = "nuiPipelineDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_create_module(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	std::string module = root["params"]["module"].asString();

	nuiModuleDescriptor *descriptor = nuiFrameworkManager::getInstance().createModule(pipeline, module);
	if (descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_pipeline(descriptor);
		response["data_type"] = "nuiPipelineDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_create_connection(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	int srcIndex = root["params"]["sourceModule"].asInt();
	int srcPort = root["params"]["sourcePort"].asInt();
	int dstIndex = root["params"]["destinationModule"].asInt();
	int dstPort = root["params"]["destinationPort"].asInt();

	nuiDataStreamDescriptor* descriptor = nuiFrameworkManager::getInstance().
		createConnection(pipeline, srcIndex, dstIndex, srcPort, dstPort);

	if (descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_connection(descriptor);
		response["data_type"] = "nuiConnectionDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_update_pipeline(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	std::string newName = root["params"]["name"].asString();
	std::string newDescription = root["params"]["description"].asString();
	std::string newAuthor = root["params"]["author"].asString();

	nuiModuleDescriptor* descr = nuiFrameworkManager::getInstance().getPipelineDescriptor(pipeline);

	if (descr == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		if (newName != "") descr->setName(newName);
		descr->setAuthor(newAuthor);
		descr->setDescription(newDescription);
		nuiModuleDescriptor* res = nuiFrameworkManager::getInstance().updatePipeline(pipeline, descr);
		setSuccess(response);
		response["result"] = serialize_pipeline(res);
		response["data_type"] = "nuiPipelineDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_update_pipelineProperty(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	std::string key = root["params"]["key"].asString();
	std::string value = root["params"]["value"].asString();

	nuiModuleDescriptor* descr = nuiFrameworkManager::getInstance().getPipelineDescriptor(pipeline);

	std::map<std::string, nuiProperty*> props = descr->getProperties();
	std::map<std::string, nuiProperty*>::iterator property = props.find(key);
	property->second->set(value);

	nuiModuleDescriptor* descriptor =
		nuiFrameworkManager::getInstance().updatePipeline(pipeline, descr);

	if (descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_pipeline(descriptor);
		response["data_type"] = "nuiPipelineDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_update_moduleProperty(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	std::string key = root["params"]["key"].asString();
	std::string value = root["params"]["value"].asString();
	int moduleIndex = root["params"]["index"].asInt();

	nuiModuleDescriptor* descr = nuiFrameworkManager::getInstance().getModuleDescriptor(pipeline, moduleIndex);

	if (descr != NULL)
	{
		std::map<std::string, nuiProperty*> props = descr->getProperties();
		std::map<std::string, nuiProperty*>::iterator property = props.find(key);
		property->second->set(value);

		nuiModuleDescriptor* descriptor =
			nuiFrameworkManager::getInstance().updateModule(pipeline, moduleIndex, descr);

		setSuccess(response);
		response["result"] = serialize_module(descriptor);
		response["data_type"] = "nuiModuleDescriptor";
	}
	else
	{
		setFailure(response);
	}
	return true;
}

bool nuiJsonRpcApi::nui_update_endpoint(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	std::string type = root["params"]["type"].asString();
	std::string newDescriptor = root["params"]["newDescriptor"].asString();
	int index = root["params"]["index"].asInt();
	int newIndex = root["params"]["newIndex"].asInt();

	nuiEndpointDescriptor* current = NULL;
	nuiEndpointDescriptor* descriptor = NULL;
	if (type == "input")
	{
		current = nuiFrameworkManager::getInstance().getInputEndpointDescriptor(pipeline, index);
		if (current == NULL)
		{
			setFailure(response);
			return false;
		}
		current->setDescriptor(newDescriptor);
		current->setIndex(newIndex);
		descriptor = nuiFrameworkManager::getInstance().updateInputEndpoint(pipeline, index, current);
	}
	else if (type == "output")
	{
		current = nuiFrameworkManager::getInstance().getOutputEndpointDescriptor(pipeline, index);
		if (current == NULL)
		{
			setFailure(response);
			return false;
		}
		current->setDescriptor(newDescriptor);
		current->setIndex(newIndex);
		descriptor = nuiFrameworkManager::getInstance().updateOutputEndpoint(pipeline, index, current);
	}
	if (descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_endpoint(descriptor);
		response["data_type"] = "nuiEndpointDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_update_connection(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	int srcIndex = root["params"]["sourceModule"].asInt();
	int srcPort = root["params"]["sourcePort"].asInt();
	int dstIndex = root["params"]["destinationModule"].asInt();
	int dstPort = root["params"]["destinationPort"].asInt();

	nuiDataStreamDescriptor *current = nuiFrameworkManager::getInstance().
		getConnectionDescriptor(pipeline, srcIndex, dstIndex, srcPort, dstPort);

	if (current == NULL)
	{
		setFailure(response);
		return false;
	}

	current->deepCopy = root["params"]["deepCopy"].asInt();
	current->asyncMode = root["params"]["asyncMode"].asInt();
	current->buffered = root["params"]["buffered"].asInt();
	current->bufferSize = root["params"]["bufferSize"].asInt();
	current->lastPacket = root["params"]["lastPacket"].asInt();
	current->overflow = root["params"]["overflow"].asInt();

	nuiDataStreamDescriptor *descriptor = nuiFrameworkManager::getInstance().
		updateConnection(pipeline, srcIndex, dstIndex, srcPort, dstPort, current);

	if (descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_connection(descriptor);
		response["data_type"] = "nuiConnectionDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_update_endpointCount(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	std::string type = root["params"]["type"].asString();
	int newCount = root["params"]["count"].asInt();

	nuiModuleDescriptor* current = NULL;
	int countUpdated = -1;
	if (type == "input")
	{
		current = nuiFrameworkManager::getInstance().getPipelineDescriptor(pipeline);
		if (current == NULL)
		{
			setFailure(response);
			return false;
		}
		countUpdated = nuiFrameworkManager::getInstance().setInputEndpointCount(pipeline, newCount);
	}
	else if (type == "output")
	{
		current = nuiFrameworkManager::getInstance().getPipelineDescriptor(pipeline);
		if (current == NULL)
		{
			setFailure(response);
			return false;
		}
		countUpdated = nuiFrameworkManager::getInstance().setOutputEndpointCount(pipeline, newCount);
	}
	if (countUpdated == -1)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = countUpdated;
		response["data_type"] = "int";
		return true;
	}
}

bool nuiJsonRpcApi::nui_delete_pipeline(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	nuiFrameworkManagerErrorCode::err error =
		nuiFrameworkManager::getInstance().deletePipeline(pipeline);
	if (error != nuiFrameworkManagerErrorCode::Success)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		return true;
	}
}

bool nuiJsonRpcApi::nui_delete_module(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	int index = root["params"]["moduleId"].asInt();

	nuiModuleDescriptor* descriptor =
		nuiFrameworkManager::getInstance().deleteModule(pipeline, index);

	if (descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_pipeline(descriptor);
		response["data_type"] = "nuiPipelineDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_delete_endpoint(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	std::string type = root["params"]["type"].asString();
	int index = root["params"]["index"].asInt();

	nuiModuleDescriptor* descriptor = NULL;
	if (type == "input")
	{
		descriptor = nuiFrameworkManager::getInstance().deleteInputEndpoint(pipeline, index);
	}
	else if (type == "output")
	{
		descriptor = nuiFrameworkManager::getInstance().deleteOutputEndpoint(pipeline, index);
	}

	if (descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_pipeline(descriptor);
		response["data_type"] = "nuiPipelineDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_delete_connection(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	int srcIndex = root["params"]["sourceModule"].asInt();
	int srcPort = root["params"]["sourcePort"].asInt();
	int dstIndex = root["params"]["destinationModule"].asInt();
	int dstPort = root["params"]["destinationPort"].asInt();

	nuiModuleDescriptor* descriptor = nuiFrameworkManager::getInstance().
		deleteConnection(pipeline, srcIndex, dstIndex, srcPort, dstPort);
	if (descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_pipeline(descriptor);
		response["data_type"] = "nuiPipelineDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_get_current(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	nuiModuleDescriptor* descriptor = nuiFrameworkManager::getInstance().getCurrentPipelineDescriptor();
	if (descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_pipeline(descriptor);
		response["data_type"] = "nuiPipelineDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_get_pipeline(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	nuiModuleDescriptor* descriptor = nuiFrameworkManager::getInstance().getPipelineDescriptor(pipeline);
	if (descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_pipeline(descriptor);
		response["data_type"] = "nuiPipelineDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_get_module(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	int index = root["params"]["identifier"].asInt();
	nuiModuleDescriptor* descriptor = nuiFrameworkManager::getInstance().getModuleDescriptor(pipeline, index);
	if (descriptor == NULL)
	{
		setFailure(response, "couldn't find module");
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_module(descriptor);
		response["data_type"] = "nuiModuleDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_get_module_properties(const Json::Value & root, Json::Value & response)
{
	/*response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	int moduleIndex = root["params"]["index"].asInt();

	nuiModuleDescriptor* descr = nuiFrameworkManager::getInstance().getModuleDescriptor(pipeline, moduleIndex);

	descr->get*/
	return false;
}

bool nuiJsonRpcApi::nui_get_availible_modules(const Json::Value & root, Json::Value & response)
{
	response["id"] = root["id"];
	std::vector<nuiModuleLoaded*>& modules = nuiPluginManager::getInstance().getLoadedModules();
	Json::Value loadedmods;

	for (std::vector<nuiModuleLoaded*>::iterator it = modules.begin(); it < modules.end(); it++)
	{
		loadedmods.append(serialize_module((*it)->getDescriptor()));
	}

	response["result"] = loadedmods;

	setSuccess(response);
	response["data_type"] = "nuiModuleLoaded_List";
	return true;
}

bool nuiJsonRpcApi::nui_get_connection(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	int srcIndex = root["params"]["sourceModule"].asInt();
	int srcPort = root["params"]["sourcePort"].asInt();
	int dstIndex = root["params"]["destinationModule"].asInt();
	int dstPort = root["params"]["destinationPort"].asInt();

	nuiDataStreamDescriptor* descriptor = nuiFrameworkManager::getInstance().
		getConnectionDescriptor(pipeline, srcIndex, dstIndex, srcPort, dstPort);

	if (descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_connection(descriptor);
		response["data_type"] = "nuiConnectionDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_navigate_push(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	int index = root["params"]["index"].asInt();

	nuiModuleDescriptor* descriptor = nuiFrameworkManager::getInstance().navigatePush(index);

	if (descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_pipeline(descriptor);
		response["data_type"] = "nuiPipelineDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_navigate_pop(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	nuiModuleDescriptor* descriptor = nuiFrameworkManager::getInstance().navigatePop();

	if (descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["result"] = serialize_pipeline(descriptor);
		response["data_type"] = "nuiPipelineDescriptor";
		return true;
	}
}

bool nuiJsonRpcApi::nui_save_workflow(const Json::Value& root, Json::Value& response)
{
	response["id"] = root["id"];
	response = serialize_workflow(nuiFrameworkManager::getInstance().getWorkflowRootDescriptor());
	setSuccess(response);
	return true;
}

bool nuiJsonRpcApi::nui_save_configuration(const Json::Value & root, Json::Value & response)
{
	response["id"] = root["id"];

	nuiFrameworkManager::getInstance().saveSettings(USER_CONFIG_PATH);

	setSuccess(response);
	return true;
}

bool nuiJsonRpcApi::nui_commit_pipeline(const Json::Value & root, Json::Value & response)
{
	response["id"] = root["id"];

	Json::Value params = root.get("params", Json::Value());
	nuiModuleDescriptor* newPipeline = deserialize_pipeline(params);

	nuiFrameworkManager::getInstance().closePipeline(newPipeline->getName());
	nuiFrameworkManager::getInstance().init(newPipeline);

	setSuccess(response);
	return true;
}

Json::Value nuiJsonRpcApi::serialize_workflow(nuiModuleDescriptor* descriptor)
{
	Json::Value jWorkflow;
	jWorkflow["name"] = descriptor->getName();
	jWorkflow["author"] = descriptor->getAuthor();
	jWorkflow["description"] = descriptor->getDescription();

	Json::Value* jPipelines = new Json::Value();
	for (int i = 0; i < descriptor->getChildModulesCount(); i++)
		jPipelines->append(serialize_pipeline(descriptor->getChildModuleDescriptor(i)));
	jWorkflow["pipelines"] = *jPipelines;

	return jWorkflow;
}

Json::Value nuiJsonRpcApi::serialize_pipeline(nuiModuleDescriptor* descriptor)
{
	Json::Value jPipeline;
	jPipeline["name"] = descriptor->getName();
	jPipeline["author"] = descriptor->getAuthor();
	jPipeline["description"] = descriptor->getDescription();

	Json::Value* jModules = new Json::Value();
	for (int i = 0; i < descriptor->getChildModulesCount(); i++)
		jModules->append(serialize_module(descriptor->getChildModuleDescriptor(i)));
	jPipeline["modules"] = *jModules;

	Json::Value* jInputEndpoints = new Json::Value();
	for (int i = 0; i < descriptor->getInputEndpointsCount(); i++)
		jInputEndpoints->append(serialize_endpoint(descriptor->getInputEndpointDescriptor(i)));
	jPipeline["inputEndpoints"] = *jInputEndpoints;

	Json::Value* jOutputEndpoints = new Json::Value();
	for (int i = 0; i < descriptor->getOutputEndpointsCount(); i++)
		jOutputEndpoints->append(serialize_endpoint(descriptor->getOutputEndpointDescriptor(i)));
	jPipeline["outputEndpoints"] = *jOutputEndpoints;

	Json::Value* connections = new Json::Value();
	for (int i = 0; i < descriptor->getDataStreamDescriptorCount(); i++)
		connections->append(serialize_connection(descriptor->getDataStreamDescriptor(i)));
	jPipeline["connections"] = *connections;

	return jPipeline;
}

Json::Value nuiJsonRpcApi::serialize_module(nuiModuleDescriptor* descriptor)
{
	Json::Value jModule;
	jModule["name"] = descriptor->getName();
	jModule["author"] = descriptor->getAuthor();
	jModule["description"] = descriptor->getDescription();

	Json::Value* jInputEndpoints = new Json::Value();
	for (int i = 0; i < descriptor->getInputEndpointsCount(); i++)
		jInputEndpoints->append(serialize_endpoint(descriptor->getInputEndpointDescriptor(i)));
	jModule["inputEndpoints"] = *jInputEndpoints;

	Json::Value* jOutputEndpoints = new Json::Value();
	for (int i = 0; i < descriptor->getOutputEndpointsCount(); i++)
		jOutputEndpoints->append(serialize_endpoint(descriptor->getOutputEndpointDescriptor(i)));
	jModule["outputEndpoints"] = *jOutputEndpoints;

	Json::Value* connections = new Json::Value();
	for (int i = 0; i < descriptor->getDataStreamDescriptorCount(); i++)
		connections->append(serialize_connection(descriptor->getDataStreamDescriptor(i)));
	jModule["connections"] = *connections;

	jModule["properties"] = serialize_properties(descriptor->getProperties());


	return jModule;
}

Json::Value nuiJsonRpcApi::serialize_endpoint(nuiEndpointDescriptor *descriptor)
{
	Json::Value jEndpoint;
	jEndpoint["index"] = descriptor->getIndex();
	jEndpoint["descriptor"] = descriptor->getDescriptor();

	return jEndpoint;
}

Json::Value nuiJsonRpcApi::serialize_connection(nuiDataStreamDescriptor *descriptor)
{
	Json::Value jConnection;
	jConnection["sourceModule"] = descriptor->sourceModuleID;
	jConnection["sourcePort"] = descriptor->sourcePort;
	jConnection["destinationModule"] = descriptor->destinationModuleID;
	jConnection["destinationPort"] = descriptor->destinationPort;

	jConnection["buffered"] = (int)descriptor->buffered;
	jConnection["bufferSize"] = (int)descriptor->bufferSize;
	jConnection["deepCopy"] = (int)descriptor->deepCopy;
	jConnection["asyncMode"] = (int)descriptor->asyncMode;
	jConnection["lastPacket"] = (int)descriptor->lastPacket;
	jConnection["overflow"] = (int)descriptor->overflow;

	return jConnection;
}

Json::Value nuiJsonRpcApi::serialize_properties(std::map<std::string, nuiProperty*> props)
{
	Json::Value jProps;

	size_t i = 0;
	for (std::map<std::string, nuiProperty*>::iterator it = props.begin();
		it != props.end(); it++)
	{
		nuiProperty* p = it->second;

		Json::Value prop;
		prop["type"] = p->getType();
		prop["name"] = it->first;
		prop["value"] = p->asString();
		prop["description"] = p->getDescription();
		jProps.append(prop);
	}

	return jProps;
}

nuiModuleDescriptor * nuiJsonRpcApi::deserialize_workflow(Json::Value & value)
{
	nuiModuleDescriptor* workflow = new nuiModuleDescriptor();

	workflow->setName(value.get("name", "").asString());
	workflow->setAuthor(value.get("author", "").asString());
	workflow->setDescription(value.get("description", "").asString());

	Json::Value& jPipelines = value.get("pipeline", Json::Value());

	if (jPipelines.isArray())
		for (Json::Value::iterator it = jPipelines.begin(); it != jPipelines.end(); it++)
		{
			workflow->addChildModuleDescriptor(deserialize_pipeline(*it));
		}

	return workflow;
}

nuiModuleDescriptor * nuiJsonRpcApi::deserialize_pipeline(Json::Value & value)
{
	nuiModuleDescriptor* pipeline = new nuiModuleDescriptor();

	pipeline->setName(value.get("name", "").asString());
	pipeline->setAuthor(value.get("author", "").asString());
	pipeline->setDescription(value.get("description", "").asString());

	Json::Value& jModules = value.get("modules", Json::Value());
	if (jModules.isArray())
		for (Json::Value::iterator it = jModules.begin(); it != jModules.end(); it++)
		{
			pipeline->addChildModuleDescriptor(deserialize_module(*it));
		}

	Json::Value& jInputEndpoints = value.get("inputEndpoints", Json::Value());
	if (jInputEndpoints.isArray())
		for (Json::Value::iterator it = jInputEndpoints.begin(); it != jInputEndpoints.end(); it++)
		{
			int index = 0;
			pipeline->addInputEndpointDescriptor(deserialize_endpoint(*it, index), index);
		}	

	Json::Value& jOutputEndpoints = value.get("outputEndpoints", Json::Value());
	if (jOutputEndpoints.isArray())
		for (Json::Value::iterator it = jOutputEndpoints.begin(); it != jOutputEndpoints.end(); it++)
		{
			int index = 0;
			pipeline->addOutputEndpointDescriptor(deserialize_endpoint(*it, index), index);
		}	

	Json::Value& connections = value.get("connections", Json::Value());
	if (connections.isArray())
		for (Json::Value::iterator it = connections.begin(); it != connections.end(); it++)
		{
			pipeline->addDataStreamDescriptor(deserialize_connection(*it));
		}	

	return pipeline;
}

nuiModuleDescriptor * nuiJsonRpcApi::deserialize_module(Json::Value & value)
{
	nuiModuleDescriptor* module = new nuiModuleDescriptor();

	module->setName(value.get("name", "").asString());
	module->setAuthor(value.get("author", "").asString());
	module->setDescription(value.get("description", "").asString());

	Json::Value& jInputEndpoints = value.get("inputEndpoints", Json::Value());
	if (jInputEndpoints.isArray())
		for (Json::Value::iterator it = jInputEndpoints.begin(); it != jInputEndpoints.end(); it++)
		{
			int index = 0;
			module->addInputEndpointDescriptor(deserialize_endpoint(*it, index), index);
		}

	Json::Value& jOutputEndpoints = value.get("outputEndpoints", Json::Value());
	if (jOutputEndpoints.isArray())
		for (Json::Value::iterator it = jOutputEndpoints.begin(); it != jOutputEndpoints.end(); it++)
		{
			int index = 0;
			module->addOutputEndpointDescriptor(deserialize_endpoint(*it, index), index);
		}

	Json::Value& connections = value.get("connections", Json::Value());
	if (connections.isArray())
		for (Json::Value::iterator it = connections.begin(); it != connections.end(); it++)
		{
			module->addDataStreamDescriptor(deserialize_connection(*it));
		}

	deserialize_properties(value.get("properties", Json::Value()), module->getProperties());

	return module;
}

nuiEndpointDescriptor * nuiJsonRpcApi::deserialize_endpoint(Json::Value & value, int& index)
{
	std::string descriptor = value.get("descriptor", "").asString();
	nuiEndpointDescriptor* endpoint = new nuiEndpointDescriptor(descriptor);
	index = value.get("index", "").asInt();

	return endpoint;
}

nuiDataStreamDescriptor * nuiJsonRpcApi::deserialize_connection(Json::Value & value)
{
	nuiDataStreamDescriptor* dataStream = new nuiDataStreamDescriptor();

	dataStream->sourceModuleID = value.get("sourceModule", 0).asInt();
	dataStream->sourcePort = value.get("sourcePort", 0).asInt();
	dataStream->destinationModuleID = value.get("destinationModule", 0).asInt();
	dataStream->destinationPort = value.get("destinationPort", 0).asInt();

	dataStream->buffered = value.get("buffered", 0).asInt();
	dataStream->bufferSize = value.get("bufferSize", 0).asInt();
	dataStream->deepCopy = value.get("deepCopy", 0).asInt();
	dataStream->asyncMode = value.get("asyncMode", 0).asInt();
	dataStream->lastPacket = value.get("lastPacket", 0).asInt();
	dataStream->overflow = value.get("overflow", 0).asInt();

	return dataStream;
}

void nuiJsonRpcApi::deserialize_properties(Json::Value& value, std::map<std::string, nuiProperty*>& props)
{
	if (value.isArray())
	{
		for (Json::Value::iterator it = value.begin(); it != value.end(); it++)
		{
			nuiPropertyType type = static_cast<nuiPropertyType>((*it).get("type", 0).asInt());
			std::string description = (*it).get("description", "").asString();
			nuiProperty* prop = new nuiProperty(type, description);

			prop->set((*it).get("value", "").asString(), false);
			std::string key = (*it).get("name", "").asString();
			if (key != "")
			{
				props.emplace(key, prop);
			}
		}
	}
}

void nuiJsonRpcApi::setFailure(Json::Value &response)
{
	response["status"] = "failure";
}

void nuiJsonRpcApi::setFailure(Json::Value &response, std::string message)
{
	response["message"] = message;
	setFailure(response);
}

void nuiJsonRpcApi::setSuccess(Json::Value &response)
{
	response["status"] = "success";
}
