using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NuiApiWrapper
{
    public class PipelineDescriptor
    {
        public string author { get; set; }
        public ConnectionDescriptor[] connections { get; set; }
        public string description { get; set; }
        public EndpointDescriptor[] inputEndpoints { get; set; }
        public ModuleDescriptor[] modules { get; set; }
        public string name { get; set; }
        public EndpointDescriptor[] outputEndpoints { get; set; }
    }
    
}
