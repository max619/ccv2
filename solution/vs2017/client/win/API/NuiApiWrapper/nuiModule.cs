using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using System.Xml.Serialization;
using System.Xml;
using System.Xml.Schema;

namespace NuiApiWrapper
{
    public class ModuleDescriptor
    {
        /*
        "descriptor": {
            "name" : "pipeline1",
            "description" : "description",
            "author": "author",
            "inputEndpoints": [
                {
                    "index" : 1,
                    "descriptor" : "audio"
                },
                {
                    "index" : 2,
                    "descriptor" : "video"
                }],
            "outputEndpoints": [
                {
                    "index" : 1,
                    "descriptor" : "tree"
                }]
        }
        */
        public string author { get; set; }
        public ConnectionDescriptor[] connections { get; set; }
        public string description { get; set; }
        public EndpointDescriptor[] inputEndpoints { get; set; }
        public string name { get; set; }
        public EndpointDescriptor[] outputEndpoints { get; set; }
        public nuiProperty[] properties { get; set; }

        public int GetId()
        {
            return int.Parse(properties.First(x => x.name == "id").value);
        }

        public void SetId(int id)
        {
            properties.First(x => x.name == "id").value = id.ToString();
        }

        public override string ToString()
        {
            return name;
        }
        
    }
}
