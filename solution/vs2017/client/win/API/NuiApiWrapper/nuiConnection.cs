using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

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
        public int asyncMode { get; set; }
        public int bufferSize { get; set; }
        public int buffered { get; set; }
        public int deepCopy { get; set; }
        public int destinationModule { get; set; }
        public int destinationPort { get; set; }
        public int lastPacket { get; set; }
        public int overflow { get; set; }
        public int sourceModule { get; set; }
        public int sourcePort { get; set; }
    }
}
