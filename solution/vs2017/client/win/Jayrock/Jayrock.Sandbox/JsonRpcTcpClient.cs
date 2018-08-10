using Jayrock.Json;
using Jayrock.Json.Conversion;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;

namespace Jayrock.JsonRpc
{
    public class JsonRpcTcpClient : JsonRpcClient
    {

        Mutex mtx;

        Regex ipRegex = new Regex(@".+\/(\d+\.\d+\.\d+\.\d+):(\d+)");

        Socket socket;

        public JsonRpcTcpClient() : base ()
        {
            socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            mtx = new Mutex();
        }

        ~JsonRpcTcpClient()
        {
            try
            {
                socket.Dispose();
                mtx.Dispose();
            }
            catch (Exception)
            {
                
            }
        }

        public override void Connect()
        {
            var endpoint = GetEndpointFromUrl(Url);
            socket.Connect(endpoint);
        }


        public override object Invoke(Type returnType, string method, object args)
        {
            if (mtx.WaitOne())
            {
                try
                {
                    if (method == null)
                        throw new ArgumentNullException("method");
                    if (method.Length == 0)
                        throw new ArgumentException(null, "method");
                    if (returnType == null)
                        throw new ArgumentNullException("returnType");

                    var sb = new StringBuilder();
                    using (var writer = new StringWriter(sb))
                    {
                        JsonObject call = new JsonObject();
                        call["id"] = ++_id;
                        call["method"] = method;
                        call["jsonrpc"] = "2.0";
                        call["params"] = args != null ? args : _zeroArgs;
                        JsonConvert.Export(call, writer);
                    }
                    //var netstring = NetstringWriter.Encode(sb.ToString());

                    var buffer = Encoding.UTF8.GetBytes(sb.ToString());
                    socket.Blocking = true;

                    socket.Send(buffer);

                    //using (Stream stream = socket.Send)
                    //{
                    //    
                    //}
                    using (var ms = new MemoryStream())
                    {
                        int offset = 0;
                        int read = 0;
                        try
                        {
                            while ((read = socket.Receive(buffer, buffer.Length, SocketFlags.None)) > 0)
                            {
                                ms.Write(buffer, 0, read);
                                socket.Blocking = false;
                                offset += read;
                            }
                        }
                        catch (SocketException)
                        {

                        }
                        ms.Position = 0;
                        using (StreamReader reader = new StreamReader(ms, Encoding.UTF8))
                            return OnResponse(JsonText.CreateReader(reader), returnType);

                    }
                }
                finally
                {
                    mtx.ReleaseMutex();
                }
            }
            return null;
        }

        private IPEndPoint GetEndpointFromUrl(string url)
        {
            var match = ipRegex.Match(url);
            if (!match.Success)
                throw new ArgumentException("Wrong ip endpoint format", nameof(Url));

            var ip = match.Groups[1].Value;
            var port = int.Parse(match.Groups[2].Value);
            return new IPEndPoint(IPAddress.Parse(ip), port);
        }
    }
}
