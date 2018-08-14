using Microsoft.VisualStudio.TestTools.UnitTesting;
using NuiApiWrapper;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NuiApiWrapper.Tests
{
    [TestClass()]
    public class NuiStateTests
    {
        public NuiStateTests()
        {
            NuiState.Instance.Connect("tcp://127.0.0.1:7500");
        }

        [TestMethod()]
        public void NavigatePushTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void NavigatePopTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void ListDynamicTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void ListPipelineTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void UpdatePropertyTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void WorkflowStartTest()
        {
            NuiState.Instance.WorkflowStart();
        }

        [TestMethod()]
        public void WorkflowStopTest()
        {
            NuiState.Instance.WorkflowStop();
        }

        [TestMethod()]
        public void WorkflowQuitTest()
        {
            NuiState.Instance.WorkflowQuit();
        }

        [TestMethod()]
        public void CreatePipelineTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void CreateModuleTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void CreateConnectionTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void UpdatePipelineTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void UpdatePipelinePropertyTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void UpdateModulePropertyTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void UpdateEndpointTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void UpdateConnectionTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void UpdateEndpointCountTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void DeletePipelineTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void DeleteModuleTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void DeleteConnectionTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void GetPipelineTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void GetModuleTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void GetConnectionTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void SaveTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void SaveConfigurationTest()
        {
            Assert.Fail();
        }

        [TestMethod()]
        public void GetLoadedModulesTest()
        {
            var res = NuiState.Instance.GetLoadedModules();
        }

        [TestMethod()]
        public void CommitPipelineTest()
        {
            NuiState.Instance.WorkflowStop();
            var pipeline = NuiState.Instance.GetPipeline("root");
            NuiState.Instance.CommitPipeline(pipeline);
        }
    }
}