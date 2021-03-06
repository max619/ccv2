#region License, Terms and Conditions
//
// Jayrock - JSON and JSON-RPC for Microsoft .NET Framework and Mono
// Written by Atif Aziz (www.raboof.com)
// Copyright (c) 2005 Atif Aziz. All rights reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
//
#endregion

namespace Jayrock.Json.Conversion.Converters
{
    #region Imports

    using System;
    using System.Collections;
    using System.Diagnostics;
    using Jayrock.Json.Conversion;
    using System.Linq;

    #endregion

    public abstract class ImporterBase : IImporter
    {
        private readonly Type _outputType;

        protected ImporterBase(Type outputType)
        {
            if (outputType == null)
                throw new ArgumentNullException("outputType");

            _outputType = outputType;
        }

        public Type OutputType
        {
            get { return _outputType; }
        }

        public virtual object Import(ImportContext context, JsonReader reader)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            if (reader == null)
                throw new ArgumentNullException("reader");

            if (!reader.MoveToContent())
                throw new JsonException("Unexpected EOF.");

            if (reader.TokenClass == JsonTokenClass.Null)
            {
                return ImportNull(context, reader);
            }
            else if (reader.TokenClass == JsonTokenClass.String)
            {
                return ImportFromString(context, reader);
            }
            else if (reader.TokenClass == JsonTokenClass.Number)
            {
                return ImportFromNumber(context, reader);
            }
            else if (reader.TokenClass == JsonTokenClass.Boolean)
            {
                return ImportFromBoolean(context, reader);
            }
            else if (reader.TokenClass == JsonTokenClass.Array)
            {
                return ImportFromArray(context, reader);
            }
            else if (reader.TokenClass == JsonTokenClass.Object)
            {
                return ImportFromObject(context, reader);
            }
            else
            {
                throw new JsonException(string.Format("{0} not expected.", reader.TokenClass));
            }
        }

        protected virtual object ImportNull(ImportContext context, JsonReader reader)
        {
            reader.Read();
            return null;
        }

        protected virtual object ImportFromBoolean(ImportContext context, JsonReader reader)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            if (reader == null)
                throw new ArgumentNullException("reader");

            if (!reader.MoveToContent())
                throw new JsonException("Unexpected EOF.");

            var str = reader.Token.Text.ToLower().Trim();

            int number = -1;

            if (str == "true")
                return true;
            else if (str == "false")
                return false;
            else if (int.TryParse(str, out number))
                return number > 0;
            else
                return false;
        }

        protected virtual object ImportFromNumber(ImportContext context, JsonReader reader)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            if (reader == null)
                throw new ArgumentNullException("reader");

            if (!reader.MoveToContent())
                throw new JsonException("Unexpected EOF.");

            var str = reader.Token.Text.ToLower().Trim();

            if (str.IndexOf(",") >= 0 || str.IndexOf(".") >= 0)
            {
                double res;
                if (double.TryParse(str, out res))
                {
                    return res;
                }
                else
                    throw new FormatException("incorrect double format of \"" + reader.Token.Class.Name + "\"");
            }
            else
            {
                int res;
                if (int.TryParse(str, out res))
                {
                    return res;
                }
                else
                    throw new FormatException("incorrect int format of \"" + reader.Token.Class.Name + "\"");
            }
        }

        protected virtual object ImportFromString(ImportContext context, JsonReader reader)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            if (reader == null)
                throw new ArgumentNullException("reader");

            if (!reader.MoveToContent())
                throw new JsonException("Unexpected EOF.");

            var str = reader.Token.Text.Trim();
            return str;
        }
        protected virtual object ImportFromArray(ImportContext context, JsonReader reader)
        {
            Debug.Assert(context != null);
            Debug.Assert(reader != null);

            reader.Read();

            IList list = null;
            Type elementType = null;
            if (!OutputType.IsGenericType)
            {
                list = new ArrayList();
                elementType = OutputType.GetElementType();
            }
            else
            {
                list = (IList)Activator.CreateInstance(OutputType);
                elementType = OutputType.GetGenericArguments()[0];
            }

            

            while (reader.TokenClass != JsonTokenClass.EndArray)
                list.Add(context.Import(elementType, reader));
            
            return ReadReturning(reader, list);
        }
        protected virtual object ImportFromObject(ImportContext context, JsonReader reader) { return ThrowNotSupported(JsonTokenClass.Object); }

        internal static object ReadReturning(JsonReader reader, object result)
        {
            Debug.Assert(reader != null);
            reader.Read();
            return result;
        }

        protected virtual JsonException GetImportException(string jsonValueType)
        {
            return new JsonException(string.Format("Cannot import {0} from a JSON {1} value.", OutputType, jsonValueType));
        }

        private object ThrowNotSupported(JsonTokenClass clazz)
        {
            Debug.Assert(clazz != null);
            throw GetImportException(clazz.Name);
        }
    }
}