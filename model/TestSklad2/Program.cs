﻿using System;
using System.Collections.Generic;
using System.Linq;
using ExtendedXmlSerializer.Configuration;
using System.Xml;
using SkladModel;
using ExtendedXmlSerializer;
using System.IO;
using ExtendedXmlSerializer.ExtensionModel.Content;
using System.Text;
using ControlModel;
using System.Xml.Linq;
using System.Data.Common;

namespace TestSklad2
{
    class Program
    {





        static void Main(string[] args)
        {            
            SkladWrapper skladWrapper = new SkladWrapper(@"..\..\..\..\..\wms-config.xml", false);
            skladWrapper.AddLogger();
            skladWrapper.AddSklad();
            skladWrapper.AddAnts(12);
            //new MoveSort(skladWrapper).Run();
            new MoveSort(skladWrapper).Run();
            skladWrapper.SaveLog(@"..\..\..\..\..\log.xml");
            SkladLogger logger = (SkladLogger)skladWrapper.objects.First(x => x is SkladLogger);
            File.WriteAllBytes(@"..\..\..\..\..\log_unity.xml", SkladWrapper.SerializeXML(logger.logs.ToArray()));


        }
    }
}
