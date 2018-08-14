//#define TEST

using System;
using System.Collections.Generic;
using NuiApiWrapper;

using System.Collections;
using System.Net;
using Jayrock.Json;
using Jayrock.JsonRpc;

using System.Web.Services;
using Jayrock.Json.Conversion;

namespace NuiApiWrapper
{
    public class NuiState
    {
        private NuiState() { }

        private static NuiState instance;
        public static NuiState Instance
        {
            get
            {
                if (instance == null)
                    instance = new NuiState();
                return instance;
            }
        }

        JsonRpcClient client;

        //! list of modules available for building pipelines
        private List<ModuleDescriptor> availableModules;

        //! list of pipelines available for building pipelines
        private List<PipelineDescriptor> availablePipelines;

        //! current level, root is 0;
        private int level = 0;

        //! descriptor for current pipeline
        private PipelineDescriptor currentPipeline;

        public void Connect(string url = "http://localhost:8080/")
        {
            if (url.IndexOf("http") > -1)
                client = new JsonRpcClient();
            else if (url.IndexOf("tcp") > -1)
                client = new JsonRpcTcpClient();
            else
                throw new ArgumentException("Defined unknown protocol", nameof(url));
            client.Url = url;
            client.Connect();
        }


        /************************************************************************/
        /* NAVIGATE                                                             */
        /************************************************************************/
        //! move into next pipeline 

        //! TODO : discuss
        //! should really be module index and return null if we can navigate into selected module
        public PipelineDescriptor NavigatePush(int moduleIdx)
        {
            PipelineDescriptor newPipeline = (PipelineDescriptor)NuiState.Instance.client.Invoke(
                typeof(PipelineDescriptor),
                "nui_navigate_push");

            NuiState.Instance.level++;
            currentPipeline = newPipeline;

            return newPipeline;
        }

        //! move out of current pipeline
        public PipelineDescriptor NavigatePop()
        {
            PipelineDescriptor newPipeline = (PipelineDescriptor)NuiState.Instance.client.Invoke(
                typeof(PipelineDescriptor),
                "nui_navigate_pop");

            NuiState.Instance.level--;
            currentPipeline = newPipeline;

            return newPipeline;
        }


        /************************************************************************/
        /* LIST                                                                 */
        /************************************************************************/

        //! lists dynamic modules
        public string[] ListDynamic()
        {
            string[] listDynamic = (string[])(new ArrayList((ICollection)
                NuiState.Instance.client.Invoke(
                    typeof(PipelineDescriptor),
                    "nui_list_dynamic")).ToArray(typeof(string)));

            return listDynamic;
        }

        //! list pipeline modules
        public string[] ListPipeline()
        {
            string[] listPipelines = (string[])(new ArrayList((ICollection)
                NuiState.Instance.client.Invoke(
                    typeof(PipelineDescriptor),
                    "nui_list_pipeline")).ToArray(typeof(string)));

            return listPipelines;
        }

        public bool UpdateProperty(string pipeline, string key, string value, int moduleIndex)
        {
            var val = new
            {
                pipeline = pipeline,
                key = key,
                value = value,
                index = moduleIndex.ToString()
            };
            return (bool)NuiState.Instance.client.Invoke(
                    typeof(bool),
                    "nui_update_moduleProperty", val);
        }


        /************************************************************************/
        /* WORKFLOW                                                             */
        /************************************************************************/
        public bool WorkflowStart()
        {
            return ((bool)NuiState.Instance.client.Invoke(
                    typeof(bool),
                    "nui_workflow_start"));
        }

        public bool WorkflowStop()
        {
            return ((bool)NuiState.Instance.client.Invoke(
                    typeof(bool),
                    "nui_workflow_stop"));
        }

        public bool WorkflowQuit()
        {
            return ((bool)NuiState.Instance.client.Invoke(
                    typeof(bool),
                    "nui_workflow_quit"));
        }

        /************************************************************************/
        /* CREATE                                                               */
        /************************************************************************/
        //! creates entirely new pipeline descriptor
        public PipelineDescriptor CreatePipeline(string name)
        {
            PipelineDescriptor pipeline = (PipelineDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(PipelineDescriptor),
                "nui_create_pipeline",
                name);

            return pipeline;
        }

        //! creates new module descriptor in pipeline
        public ModuleDescriptor CreateModule(string pipelineName, string moduleName)
        {
            ModuleDescriptor module = (ModuleDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(PipelineDescriptor),
                "nui_create_module",
                pipelineName, moduleName);

            return module;
        }

        //! create new connection
        public bool CreateConnection(string pipelineName,
            int srcModuleIdx, int srcModulePort,
            int dstModuleIdx, int dstModulePort)
        {
            bool status = (bool)NuiState.Instance.client.InvokeVargs(
                typeof(PipelineDescriptor),
                "nui_create_module",
                pipelineName, srcModuleIdx, srcModulePort, dstModuleIdx, dstModulePort);

            return status;
        }


        /************************************************************************/
        /* UPDATE                                                               */
        /************************************************************************/
        public PipelineDescriptor UpdatePipeline(string pipelineName, string newName = null, string newDescription = null, string author = null)
        {
            PipelineDescriptor pipeline = (PipelineDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(PipelineDescriptor),
                "nui_update_pipeline",
                pipelineName, newName, newDescription, author);

            return pipeline;
        }

        public PipelineDescriptor UpdatePipelineProperty(string pipelineName,
            string key, object value, string description)
        {
            PipelineDescriptor pipeline = (PipelineDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(PipelineDescriptor),
                "nui_update_pipelineProperty",
                pipelineName, key, value, description);

            return pipeline;
        }

        public bool UpdateModuleProperty(string pipelineName,
            int moduleIdx, string key, object value)
        {
            var args = new
            {
                pipeline = pipelineName,
                key = key,
                value = value.ToString(),
                index = moduleIdx
            };
            Response<ModuleDescriptor> pipeline = (Response<ModuleDescriptor>)NuiState.Instance.client.Invoke(
                typeof(Response<ModuleDescriptor>),
                "nui_update_moduleProperty",
                args);

            return pipeline.Success;
        }

        public EndpointDescriptor UpdateEndpoint(string type, int endpointIdx, string descriptor, int newIndex = -1)
        {
            EndpointDescriptor endpoint;
            if (newIndex < 0)
            {
                endpoint = (EndpointDescriptor)NuiState.Instance.client.InvokeVargs(
                    typeof(EndpointDescriptor),
                    "nui_update_endpoint",
                    type, endpointIdx, descriptor, null);
            }
            else
            {
                endpoint = (EndpointDescriptor)NuiState.Instance.client.InvokeVargs(
                    typeof(EndpointDescriptor),
                    "nui_update_endpoint",
                    type, endpointIdx, descriptor, newIndex);
            }

            return endpoint;
        }

        public ConnectionDescriptor UpdateConnection(string pipelineName,
            int srcModuleIdx, int srcModulePort,
            int dstModuleIdx, int dstModulePort, params KeyValuePair<string, object>[] keyValues)
        {
            var connection = (ConnectionDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(ConnectionDescriptor),
                "nui_update_endpoint",
                pipelineName, srcModuleIdx, srcModulePort,
                dstModuleIdx, dstModulePort, keyValues);

            return connection;
        }

        public int UpdateEndpointCount(string pipelineName, string type, int newCount)
        {
            var count = (int)NuiState.Instance.client.InvokeVargs(
                typeof(int),
                "nui_update_endpointCount",
                pipelineName, type, newCount);

            return count;
        }


        /************************************************************************/
        /* DELETE                                                               */
        /************************************************************************/
        public bool DeletePipeline(string pipelineName)
        {
            var status = (bool)NuiState.Instance.client.InvokeVargs(
                typeof(bool),
                "nui_delete_pipeline",
                pipelineName);

            return status;
        }

        public PipelineDescriptor DeleteModule(string pipelineName, int moduleIndex)
        {
            var pipeline = (PipelineDescriptor)NuiState.Instance.client.InvokeVargs(
                 typeof(PipelineDescriptor),
                 "nui_delete_module",
                 pipelineName, moduleIndex);

            return pipeline;
        }

        public PipelineDescriptor DeleteConnection(string pipelineName,
            int srcModuleIdx, int srcModulePort,
            int dstModuleIdx, int dstModulePort)
        {
            var pipeline = (PipelineDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(PipelineDescriptor),
                "nui_delete_connection",
                pipelineName, srcModuleIdx, srcModulePort, dstModuleIdx, dstModulePort);

            return pipeline;
        }


        /************************************************************************/
        /* GET                                                                  */
        /************************************************************************/

        public PipelineDescriptor GetPipeline(string pipelineName)
        {
            var args = new { pipeline = pipelineName };
            var pipeline = (PipelineDescriptor)NuiState.Instance.client.Invoke(
                typeof(PipelineDescriptor),
                "nui_get_pipeline",
                args);

            return pipeline;
        }

        public ModuleDescriptor GetModule(string pipelineName, int moduleIdx)
        {
            var module = (ModuleDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(ModuleDescriptor),
                "nui_get_module",
                pipelineName, moduleIdx);

            return module;
        }

        public ConnectionDescriptor GetConnection(string pipelineName, int connectionIdx)
        {
            var connection = (ConnectionDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(ConnectionDescriptor),
                "nui_get_connection",
                pipelineName, connectionIdx);

            return connection;
        }


        /************************************************************************/
        /* SAVE                                                                 */
        /************************************************************************/
        public bool Save(string pipelineName, string fileName)
        {
            var response = (bool)NuiState.Instance.client.InvokeVargs(
                typeof(bool),
                "nui_save_pipeline",
                pipelineName, fileName);

            return response;
        }

        public bool SaveConfiguration()
        {
            var response = (bool)NuiState.Instance.client.Invoke(
                typeof(bool),
                "nui_save_configuration");

            return response;
        }

        public List<ModuleDescriptor> GetLoadedModules()
        {
            var response = (List<ModuleDescriptor>)NuiState.Instance.client.Invoke(
               typeof(List<ModuleDescriptor>),
               "nui_get_availible_modules");

            return response;
        }
    }
}