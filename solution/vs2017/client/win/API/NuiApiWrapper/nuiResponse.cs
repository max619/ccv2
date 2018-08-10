using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NuiApiWrapper
{
    public class Response<T>
        where T : new()
    {
        /// <summary>
        /// Success status. To get response GetResponse should be used
        /// </summary>
        public string status { get; set; }
        public bool Success => status == "success";

        /// <summary>
        /// Do NOT use directly, call GetResponse instead
        /// </summary>
        public T result { get; set; }

        public T GetResponse()
        {
            if (Success)
                return result;
            else
                return default(T);
        }
    }
}
