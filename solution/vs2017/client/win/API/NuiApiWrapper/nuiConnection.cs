using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace NuiApiWrapper
{
    /** 
     * public static string connection = @"
     * {
     * "success": 1,
     * "descriptor": {
     * "sourceModule" : 0,
     * "sourcePort" : 0,
     * "destinationModule" : 1,
     * "destinationPort": 0,
     * "deepCopy": 0,
     * "asyncMode": 0,
     * "buffered": 0,
     * "bufferSize": 0,
     * "lastPacket": 0,
     * "overflow": 0, 
     * }
     */

    public class ConnectionDescriptor
    {
        [XmlAttribute]
        public int asyncMode { get; set; }
        [XmlAttribute]
        public int bufferSize { get; set; }
        [XmlAttribute]
        public int buffered { get; set; }
        [XmlAttribute]
        public int deepCopy { get; set; }
        [XmlAttribute]
        public int destinationModule { get; set; }
        [XmlAttribute]
        public int destinationPort { get; set; }
        [XmlAttribute]
        public int lastPacket { get; set; }
        [XmlAttribute]
        public int overflow { get; set; }
        [XmlAttribute]
        public int sourceModule { get; set; }
        [XmlAttribute]
        public int sourcePort { get; set; }        

        public ConnectionDescriptor Clone()
        {
            return new ConnectionDescriptor
            {
                asyncMode = this.asyncMode,
                bufferSize = this.bufferSize,
                buffered = this.buffered,
                deepCopy = this.deepCopy,
                destinationModule = this.destinationModule,
                destinationPort = this.destinationPort,
                lastPacket = this.lastPacket,
                overflow = this.overflow,
                sourceModule = this.sourceModule,
                sourcePort = this.sourcePort
            };
        }
    }
}
