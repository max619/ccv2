/*

Copyright (c) 2011, Thomas ICHE
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
following conditions are met:

        * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer 
          in the documentation and/or other materials provided with the distribution.
        * Neither the name of PeeWeeK.NET nor the names of its contributors may be used to endorse or promote products derived from this 
          software without specific prior written permission.


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;
using NodeGraphControl.Utils;
using NuiApiWrapper;

namespace NodeGraphControl
{
    /// <summary>
    /// Represents a link between two NodeGraphConnectors
    /// </summary>
    public class NodeGraphLink : XmlSerializibleBase
    {
        public ConnectionDescriptor ConnectionDescriptor { get; set; }
        /// <summary>
        /// The first end of the link, that's connected to an Output Connector
        /// </summary>
        public NodeGraphConnector Input
        {
            get { return this.m_InputConnector; }
        }
        /// <summary>
        /// The last end of the link, that's connected to an Input Connector
        /// </summary>
        public NodeGraphConnector Output
        {
            get { return this.m_OutputConnector; }
        }
        private NodeGraphConnector m_InputConnector;
        private NodeGraphConnector m_OutputConnector;
        private int v_InputNodeId;
        private int v_OutputNodeId;
        private int v_InputNodeConnectorIdx;
        private int v_OutputNodeConnectorIdx;

        public NodeGraphLink()
        {

        }

        /// <summary>
        /// Creates a new NodeGraphLink, given input and output Connectors
        /// </summary>
        /// <param name="p_Input"></param>
        /// <param name="p_Output"></param>
        public NodeGraphLink(NodeGraphConnector p_Input, NodeGraphConnector p_Output)
        {
            this.m_InputConnector = p_Input;
            this.m_OutputConnector = p_Output;
        }

        /// <summary>
        /// CONVERSION: Creates a NodeGraphLink, given an XML Serialized copy of the link and a view
        /// </summary>
        /// <param name="reader"></param>
        /// <param name="p_View"></param>
        public NodeGraphLink(ConnectionDescriptor connection, NodeGraphView p_View)
        {
            CreateFromConnectionDescriptor(connection, p_View);
        }

        /// <summary>
        /// SERIALIZATION: Creates a NodeGraphLink, given an XML Serialized copy of the link and a view
        /// </summary>
        /// <param name="reader"></param>
        /// <param name="p_View"></param>
        public NodeGraphLink(XmlReader reader, NodeGraphView p_View)
        {
            ReadXml(reader);

            if (ConnectionDescriptor != null)
            {
                CreateFromConnectionDescriptor(ConnectionDescriptor, p_View);
            }
        }

        internal static NodeGraphLink FromConnectionDescriptor(ConnectionDescriptor connection, NodeGraphView nodeGraphView)
        {
            return new NodeGraphLink(connection, nodeGraphView);
        }

        public override void ReadXml(XmlReader reader)
        {
            v_InputNodeId = int.Parse(reader.GetAttribute("InputNodeId"));
            v_OutputNodeId = int.Parse(reader.GetAttribute("OutputNodeId"));
            v_InputNodeConnectorIdx = int.Parse(reader.GetAttribute("InputNodeConnectorIdx"));
            v_OutputNodeConnectorIdx = int.Parse(reader.GetAttribute("OutputNodeConnectorIdx"));

            if (TryReadTill("ConnectionDescriptor", reader))
            {
                ConnectionDescriptor = DeserializeObject<ConnectionDescriptor>(reader);
            }
        }

        public override void WriteXml(XmlWriter writer)
        {
            NodeGraphView v_View = Input.Parent.ParentView;
            NodeGraphNode v_InputNode = Input.Parent;
            NodeGraphNode v_OutputNode = Output.Parent;


            writer.WriteAttributeString("InputNodeId", (v_InputNodeId = v_View.GetNodeIndex(v_InputNode)).ToString());
            writer.WriteAttributeString("OutputNodeId", (v_OutputNodeId = v_View.GetNodeIndex(v_OutputNode)).ToString());
            writer.WriteAttributeString("InputNodeConnectorIdx", (v_InputNodeConnectorIdx = v_InputNode.GetConnectorIndex(Input)).ToString());
            writer.WriteAttributeString("OutputNodeConnectorIdx", (v_OutputNodeConnectorIdx = v_OutputNode.GetConnectorIndex(Output)).ToString());

            SerializeObject(ConnectionDescriptor, writer);
        }

        protected void CreateFromConnectionDescriptor(ConnectionDescriptor connection, NodeGraphView p_View)
        {
            v_InputNodeId = connection.sourceModule;
            v_OutputNodeId = connection.destinationModule;
            v_InputNodeConnectorIdx = connection.sourcePort;
            v_OutputNodeConnectorIdx = connection.destinationPort;

            if (v_InputNodeId != 0x0FFFFFFF)
            {
                this.m_InputConnector = p_View.NodeCollection[v_InputNodeId].GetConnector(v_InputNodeConnectorIdx, ConnectorType.OutputConnector);
            }
            else
                this.m_InputConnector = p_View.NodeConnectorCollection[v_InputNodeConnectorIdx];

            if (v_OutputNodeId != 0x0FFFFFFF)
            {
                this.m_OutputConnector = p_View.NodeCollection[v_OutputNodeId].GetConnector(v_OutputNodeConnectorIdx, ConnectorType.InputConnector);
            }
            else
                this.m_OutputConnector = p_View.NodeConnectorCollection[v_OutputNodeConnectorIdx];

            ConnectionDescriptor = connection;
        }
    }
}
