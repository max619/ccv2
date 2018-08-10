using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace NuiApiWrapper
{
    public class PipelineDescriptor
    {
        [XmlAttribute]
        public string author { get; set; }
        public List<ConnectionDescriptor> connections { get; set; }
        [XmlAttribute]
        public string description { get; set; }
        public List<EndpointDescriptor> inputEndpoints { get; set; }
        [XmlIgnore]
        public List<ModuleDescriptor> modules { get; set; }
        [XmlAttribute]
        public string name { get; set; }
        public List<EndpointDescriptor> outputEndpoints { get; set; }

        public int GetNextIndex()
        {
            int max = int.MinValue;
            modules.ForEach((x) =>
            {
                var id = x.GetId();
                if (id > max)
                    max = id;
            });
            max++;
            return max;
        }
    }

}
