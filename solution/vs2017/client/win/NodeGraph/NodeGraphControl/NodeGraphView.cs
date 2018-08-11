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
using System.IO;
using System.Windows.Forms;
using NuiApiWrapper;
using System.Xml;
using System.Xml.Serialization;
using System.Xml.Schema;
using NodeGraphControl.Utils;
using System.Linq;
using System.ComponentModel;

namespace NodeGraphControl
{
    /// <summary>
    /// Encapsulates a node view (nodes & links)
    /// </summary>
    public class NodeGraphView : XmlSerializibleBase
    {
        public PipelineDescriptor PipelineDescriptor { get; private set; }
        /// <summary>
        /// The node Collection contained in this view
        /// </summary>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public List<NodeGraphNode> NodeCollection
        {
            get { return m_NodeCollection; }
            set { m_NodeCollection = value; }
        }

        /// <summary>
        /// The collection of input and output endpoint of pipeline contained in this view
        /// </summary>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public List<NodeGraphConnector> NodeConnectorCollection
        {
            get { return m_NodeConnectorCollection; }
            set { m_NodeConnectorCollection = value; }
        }

        /// <summary>
        /// The collection of currently Selected nodes in this view
        /// </summary>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public List<NodeGraphNode> SelectedItems
        {
            get { return m_SelectedItems; }
            set { m_SelectedItems = value; }
        }
        /// <summary>
        /// The collection of Links created in this view
        /// </summary>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public List<NodeGraphLink> Links
        {
            get { return m_Links; }
            set { m_Links = value; }
        }
        /// <summary>
        /// The panel that contains this view
        /// </summary>
        public NodeGraphPanel ParentPanel
        {
            get { return m_oPanel; }
            set { m_oPanel = value; }
        }
        /// <summary>
        /// The orthographic X coordinate of the current View
        /// </summary>
        public int ViewX;
        /// <summary>
        /// The orthographic Y coordinate of the current View
        /// </summary>
        public int ViewY;
        /// <summary>
        /// The zoom factor of the current view
        /// </summary>
        public float ViewZoom;
        /// <summary>
        /// Current zoom Factor, used on smooth behavior
        /// </summary>
        public float CurrentViewZoom;

        /// <summary>
        /// Point to which we translate on zooming
        /// </summary>
        public System.Drawing.Point ZoomTranslate;


        private List<NodeGraphNode> m_NodeCollection;
        private List<NodeGraphConnector> m_NodeConnectorCollection;

        private List<NodeGraphNode> m_SelectedItems;
        private List<NodeGraphLink> m_Links;


        private NodeGraphPanel m_oPanel;

        public NodeGraphView() : this(null)
        {

        }

        /// <summary>
        /// Creates a new NodeGraphView in a NodeGraphpanel
        /// </summary>
        /// <param name="p_Panel"></param>
        public NodeGraphView(NodeGraphPanel p_Panel)
        {
            this.m_NodeCollection = new List<NodeGraphNode>();
            this.m_NodeConnectorCollection = new List<NodeGraphConnector>();
            this.m_SelectedItems = new List<NodeGraphNode>();
            this.m_Links = new List<NodeGraphLink>();
            this.ViewX = 0;
            this.ViewY = 0;
            this.ViewZoom = 1.0f;
            this.CurrentViewZoom = 1.0f;
            this.m_oPanel = p_Panel;
        }


        /// <summary>
        /// SERIALIZATION: Creates a NodeGraphView from a Pipeline Descriptor
        /// </summary>
        /// <param name="p_Pipeline">Pipeline descriptor</param>
        /// <param name="p_Panel">the panel that will contain this NodeGraphView</param>
        public NodeGraphView(PipelineDescriptor p_Pipeline, NodeGraphPanel p_Panel)
        {
            //System.Globalization.CultureInfo v_IntlCultureInfo = new System.Globalization.CultureInfo("en-us");
            PipelineDescriptor = p_Pipeline;
            this.m_oPanel = p_Panel;
            this.ViewX = 0;
            this.ViewY = 0;
            this.ViewZoom = 1.0f;

            this.m_NodeCollection = new List<NodeGraphNode>();

            foreach (var module in p_Pipeline.modules)
            {
                this.m_NodeCollection.Add(new ModuleNode(module, this));
            }

            m_NodeConnectorCollection = new List<NodeGraphConnector>();

            foreach (var endpoint in p_Pipeline.inputEndpoints)
            {
                this.m_NodeConnectorCollection.Add(new NodeGraphConnector(endpoint, this, ConnectorType.InputConnector));
            }

            foreach (var endpoint in p_Pipeline.outputEndpoints)
            {
                this.m_NodeConnectorCollection.Add(new NodeGraphConnector(endpoint, this, ConnectorType.OutputConnector));
            }

            this.m_Links = new List<NodeGraphLink>();

            foreach (var connection in p_Pipeline.connections)
            {
                this.m_Links.Add(NodeGraphLink.FromConnectionDescriptor(connection, this));
            }

            PlaceNodes();
            

            this.m_SelectedItems = new List<NodeGraphNode>();

        }

        private void PlaceNodes()
        {
            var rootNodes = GetRootNodes();

            int y = 0;

            foreach(var n in rootNodes)
            {
                int x = 0;
                SetRootNodesPosition(ref x, ref y, n);
            }

            //for (int i = 0; i < this.m_NodeCollection.Count; i++)
            //{
            //    //! TODO : normal algorythm for module placement
            //    this.m_NodeCollection[i].X = i * 100;
            //    this.m_NodeCollection[i].Y = i * 100;
            //    this.m_NodeCollection[i].UpdateHitRectangle();
            //}
        }

        private void SetRootNodesPosition(ref int x, ref int y, NodeGraphNode n)
        {
            var outputLinks = m_Links.Where(f => n.Connectors.Contains(f.Input)).ToList();
            n.X = x;
            n.Y = y;
            foreach(var link in outputLinks)
            {
                x = n.X + 250;
                var childNode = link.Output.Parent;
                SetRootNodesPosition(ref x, ref y, childNode);
                y += 200;
            }
        }

        private IEnumerable<NodeGraphNode> GetRootNodes()
        {
            foreach(var n in m_NodeCollection)
            {
                //if (n.Connectors.Where(x => x.Type == ConnectorType.InputConnector).FirstOrDefault() == null)
                //    yield return n;
                if(!m_Links.Select((x)=>x.Output.Parent).Contains(n))
                {
                    yield return n;
                }
            }
        }

        /// <summary>
        /// SERIALIZATION: Creates a NodeGraphView from a Serialized XML Copy
        /// </summary>
        /// <param name="p_XmlTreeNode">the parent XmlTreeNode used in serialization</param>
        /// <param name="p_Panel">the panel that will contain this NodeGraphView</param>
        public NodeGraphView(XmlReader p_XmlTreeNode, NodeGraphPanel p_Panel)
        {
            this.m_oPanel = p_Panel;
            this.ViewX = 0;
            this.ViewY = 0;
            this.ViewZoom = 1.0f;
            ReadXml(p_XmlTreeNode);
            m_SelectedItems = new List<NodeGraphNode>();
        }

        /// <summary>
        /// Returns the Node Index of the NodeGraphNode in this view's collection
        /// </summary>
        /// <param name="p_Node"></param>
        /// <returns></returns>
        public int GetNodeIndex(NodeGraphNode p_Node)
        {
            for (int i = 0; i < this.m_NodeCollection.Count; i++)
            {
                if (this.m_NodeCollection[i] == p_Node) return i;
            }
            return -1;
        }

        /// <summary>
        /// Returns the Node Index of the NodeGraphNode in this view's current selection
        /// </summary>
        /// <param name="p_Node"></param>
        /// <returns></returns>
        public int GetSelectionNodeIndex(NodeGraphNode p_Node)
        {
            for (int i = 0; i < this.m_SelectedItems.Count; i++)
            {
                if (this.m_SelectedItems[i] == p_Node) return i;
            }
            return -1;
        }

        public override void ReadXml(XmlReader reader)
        {
            System.Globalization.CultureInfo v_IntlCultureInfo = new System.Globalization.CultureInfo("en-us");
            ReadTill("NodeGraphView", reader);
            this.ViewX = int.Parse(reader.GetAttribute("ViewX"));
            this.ViewY = int.Parse(reader.GetAttribute("ViewY"));
            this.ViewZoom = float.Parse(reader.GetAttribute("ViewZoom"));

            ReadTill("PipelineDescriptor", reader);
            PipelineDescriptor = DeserializeObject<PipelineDescriptor>(reader);

            this.m_NodeConnectorCollection = new List<NodeGraphConnector>();
            foreach (var endpoint in PipelineDescriptor.inputEndpoints)
            {
                this.m_NodeConnectorCollection.Add(new NodeGraphConnector(endpoint, this, ConnectorType.InputConnector));
            }

            foreach (var endpoint in PipelineDescriptor.outputEndpoints)
            {
                this.m_NodeConnectorCollection.Add(new NodeGraphConnector(endpoint, this, ConnectorType.OutputConnector));
            }

            ReadTill("NodeGraphNodeCollection", reader);

            this.m_NodeCollection = new List<NodeGraphNode>();

            while (TryReadTill("NodeGraphNode", reader))
            {
                var type = ReadType(reader);
                m_NodeCollection.Add((NodeGraphNode)Activator.CreateInstance(type, reader, this));
            }

            this.m_Links = new List<NodeGraphLink>();

            foreach (var connection in PipelineDescriptor.connections)
            {
                this.m_Links.Add(NodeGraphLink.FromConnectionDescriptor(connection, this));
            }
            //while (TryReadTill("NodeGraphLink", reader))
            //{
            //    m_Links.Add((NodeGraphLink)Activator.CreateInstance(typeof(NodeGraphLink), reader, this));
            //}
            for (int i = 0; i < this.m_NodeCollection.Count; i++)
            {
                this.m_NodeCollection[i].UpdateHitRectangle();
            }
            RestorePipeline();
        }

        public override void WriteXml(XmlWriter writer)
        {
            writer.WriteAttributeString("ViewX", ViewX.ToString());
            writer.WriteAttributeString("ViewY", ViewY.ToString());
            writer.WriteAttributeString("ViewZoom", ViewZoom.ToString());
            writer.WriteAttributeString("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");

            SerializeObject(PipelineDescriptor, writer);

            writer.WriteStartElement("NodeGraphNodeCollection");
            foreach (NodeGraphNode i_Node in this.NodeCollection)
            {
                SerializeObject(i_Node, writer);
            }
            writer.WriteEndElement();

        }


        /// <summary>
        /// CLIPBOARD: Copies the selection as a list of Nodes into ClipBoard.
        /// </summary>
        public void CopySelectionToClipboard()
        {
            string v_TempPath = Path.GetTempPath() + "NodeGraphClipboard.xml";

            using (var str = File.Create(v_TempPath))
            using (var writer = XmlWriter.Create(str))
            {
                writer.WriteStartDocument();
                writer.WriteStartElement("Clipboard");
                writer.WriteAttributeString("X", ParentPanel.ViewSpaceMousePosition.X.ToString());
                writer.WriteAttributeString("Y", ParentPanel.ViewSpaceMousePosition.Y.ToString());

                writer.WriteStartElement("SelectedNodes");
                foreach (NodeGraphNode i_Node in this.m_SelectedItems)
                {
                    SerializeObject(i_Node, writer);
                }
                writer.WriteEndElement();

                writer.WriteStartElement("Links");
                foreach (NodeGraphLink i_Link in this.m_Links)
                {
                    // if the node is connecting copied nodes
                    if (this.m_SelectedItems.Contains(i_Link.Input.Parent) && this.m_SelectedItems.Contains(i_Link.Output.Parent))
                    {
                        writer.WriteStartElement("ToBeRelinked");

                        writer.WriteAttributeString("InputNodeId", this.GetSelectionNodeIndex(i_Link.Input.Parent).ToString());
                        writer.WriteAttributeString("InputNodeConnectorIdx", i_Link.Input.Parent.GetConnectorIndex(i_Link.Input).ToString());
                        writer.WriteAttributeString("OutputNodeId", this.GetSelectionNodeIndex(i_Link.Output.Parent).ToString());
                        writer.WriteAttributeString("OutputNodeConnectorIdx", i_Link.Output.Parent.GetConnectorIndex(i_Link.Output).ToString());

                        SerializeObject(i_Link.ConnectionDescriptor, writer);

                        writer.WriteEndElement();
                    }
                }
                writer.WriteEndElement();

                writer.WriteEndElement();
                writer.WriteEndDocument();
            }

            Clipboard.Clear();
            System.Collections.Specialized.StringCollection v_ClipBoardContent = new System.Collections.Specialized.StringCollection();
            v_ClipBoardContent.Add(v_TempPath);
            Clipboard.SetFileDropList(v_ClipBoardContent);

        }
        /// <summary>
        /// CLIPBOARD: If contains a NodeGraphClipboard.xml, deserializes and add nodes to current view.
        /// </summary>
        public void PasteSelectionFromClipBoard()
        {
            if (Clipboard.ContainsFileDropList())
            {
                if (Clipboard.GetFileDropList().Contains(Path.GetTempPath() + "NodeGraphClipboard.xml"))
                {
                    string v_TempPath = Path.GetTempPath() + "NodeGraphClipboard.xml";
                    int PreviousNodeCount = this.m_NodeCollection.Count;
                    using (var reader = XmlReader.Create(v_TempPath))
                    {
                        ReadTill("Clipboard", reader);
                        int old_x = int.Parse(reader.GetAttribute("X"));
                        int old_y = int.Parse(reader.GetAttribute("Y"));
                        int dx = ParentPanel.ViewSpaceMousePosition.X - old_x;
                        int dy = ParentPanel.ViewSpaceMousePosition.Y - old_y;
                        ReadTill("SelectedNodes", reader);                        
                        while (TryReadTill("NodeGraphNode", reader))
                        {
                            var type = ReadType(reader);
                            var node = (NodeGraphNode)Activator.CreateInstance(type, reader, this);
                            node.X += dx;
                            node.Y += dy;
                            if(node is ModuleNode)
                            {
                                var m = (ModuleNode)node;
                                m.Descriptor.SetId(PipelineDescriptor.GetNextIndex());
                            }
                            node.UpdateHitRectangle();
                            this.NodeCollection.Add(node);
                        }


                        int v_InId, n_In, n_Ou, v_InConnectorIdx, v_OutId, v_OutConnectorIdx;
                        ReadTill("Links", reader);
                        while (TryReadTill("ToBeRelinked", reader))
                        {
                            v_InId = int.Parse(reader.GetAttribute("InputNodeId"));
                            n_In = v_InId + PreviousNodeCount;
                            v_InConnectorIdx = int.Parse(reader.GetAttribute("InputNodeConnectorIdx"));
                            v_OutId = int.Parse(reader.GetAttribute("OutputNodeId"));
                            n_Ou = v_OutId + PreviousNodeCount;
                            v_OutConnectorIdx = int.Parse(reader.GetAttribute("OutputNodeConnectorIdx"));

                            ReadTill("ConnectionDescriptor", reader);

                            var descriptor = DeserializeObject<ConnectionDescriptor>(reader);
                            descriptor.destinationModule = n_Ou;
                            descriptor.sourceModule = n_In;

                            this.m_Links.Add(new NodeGraphLink(descriptor, this));
                            PipelineDescriptor.connections.Add(descriptor);

                            reader.Read();
                        }
                    }
                }
            }
            ParentPanel.Refresh();
        }

        private void RestorePipeline()
        {
            PipelineDescriptor.modules = m_NodeCollection.Where((x) => x is ModuleNode).Cast<ModuleNode>().Select((x) => x.Descriptor).ToList();
        }

        public NodeGraphConnector CreateConnectorByModuleDescriptors(int NodeId, int NodeConnector, ConnectorType type)
        {
            EndpointDescriptor descriptor = null;
            var module = m_NodeCollection.Where((x) => x is ModuleNode).Cast<ModuleNode>()
                .FirstOrDefault((x) => int.Parse(x.Descriptor.properties.First((y) => y.name == "id").value) == NodeId);
            if (module != null)
            {
                if (type == ConnectorType.OutputConnector && module.Descriptor.outputEndpoints != null)
                    descriptor = module.Descriptor.outputEndpoints.Where(x => x.index == NodeConnector).FirstOrDefault();
                else if (type == ConnectorType.InputConnector && module.Descriptor.inputEndpoints != null)
                    descriptor = module.Descriptor.inputEndpoints.Where(x => x.index == NodeConnector).FirstOrDefault();

                var connector = new NodeGraphConnector(descriptor, module, type);
                m_NodeConnectorCollection.Add(connector);
                return connector;
            }
            throw new ArgumentException("Module with id=" + NodeId + " not found");
        }

    }
}

