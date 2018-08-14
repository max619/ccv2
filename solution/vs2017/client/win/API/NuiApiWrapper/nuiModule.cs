using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using System.Xml.Serialization;
using System.Xml;
using System.Xml.Schema;
using System.ComponentModel;

namespace NuiApiWrapper
{
    public class ModuleDescriptor
    {
        [Category("Info"), DisplayName("Author")]
        [ReadOnly(true)]
        public string author { get; set; }

        [Category("Info"), DisplayName("Name")]
        [ReadOnly(true)]
        public string name { get; set; }

        [Category("Info"), DisplayName("Description")]
        [ReadOnly(true)]
        public string description { get; set; }

        [Browsable(false)]
        public List<ConnectionDescriptor> connections { get; set; }

        [Browsable(false)]
        public EndpointDescriptor[] inputEndpoints { get; set; }

        [Browsable(false)]
        public EndpointDescriptor[] outputEndpoints { get; set; }

        [ReadOnly(true)]
        public List<nuiProperty> properties { get; set; }

        [Category("Data"), DisplayName("Input endpoints")]
        public int InputEndpounts => inputEndpoints != null ? inputEndpoints.Length : 0;

        [Category("Data"), DisplayName("Output endpoints")]
        public int OutputEndpoints => outputEndpoints != null ? outputEndpoints.Length : 0;

        [Category("Data"), DisplayName("Connections")]
        public int Connections => connections != null ? connections.Count : 0;

        public int GetId()
        {
            return int.Parse(properties.First(x => x.name == "id").value);
        }

        public void SetId(int id)
        {
            var prop = properties.FirstOrDefault(x => x.name == "id");
            if (prop != null)
            {
                prop.value = id.ToString();
            }
            else
            {
                properties.Add(new nuiProperty
                {
                    name = "id",
                    type = (int)nuiPropertyType.NUI_PROPERTY_INTEGER,
                    value = id.ToString()
                });
            }
        }

        public override string ToString()
        {
            return name;
        }

    }
}
