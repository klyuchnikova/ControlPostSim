using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using SkladModel;
using System.Linq;
using ExtendedXmlSerializer.Configuration;
using ExtendedXmlSerializer;
using System.IO;
using System.Xml;
using System;
using Object = UnityEngine.Object;
using Unity.VisualScripting;

public class main : MonoBehaviour
{

    public static Vector2 getPosition(double x, double y)
    {
        return new Vector2((float)x - 10, -(float)y+8);
    }

    public Transform[] brick;
    public Transform AntBotTransform;
    Dictionary<string, AntBotUnity> antsBot = new Dictionary<string, AntBotUnity>();
    private SkladWrapper skladWrapper;
    Sklad sklad;
    private SkladLogger logger;
    DateTime startTime;
    AntStateChange asc;
    public Transform panel;

    // Start is called before the first frame update
    void Start()
    {
        System.Random rnd = new System.Random(DateTime.Now.Millisecond);
        SkladWrapper skladWrapper = new SkladWrapper(@"skladConfig.xml");
        while (skladWrapper.Next())
        {
            List<AntBot> freeAnts = skladWrapper.GetFreeAnts();
            if (freeAnts.Count > 0 && skladWrapper.isEventCountEmpty())
            {
                foreach(AntBot ant in freeAnts)
                {
                    if (ant.charge > 7100)
                        if (ant.commandList.commands.Count == 0)
                        {
                            Direction dir = (Direction)rnd.Next(0, 4);
                            int pm = skladWrapper.getFreePath(ant, dir, ant.lastUpdated);
                            int count = 0;
                            while (pm == 0)
                            {
                                count++;
                                if (count > 10)
                                    break;
                                dir = (Direction)rnd.Next(0, 4);
                                pm = skladWrapper.getFreePath(ant, dir, ant.lastUpdated);
                            }
                            if (pm != 0)
                            {
                                skladWrapper.Move(ant, dir, pm);
                                break;
                            }
                        }
                }

            }
        }

        sklad = (Sklad) skladWrapper.objects.First(x=>x is Sklad);

        foreach (var yl in sklad.skladLayout)
        {
            int y = yl.Key;
            foreach (var xl in yl.Value) {
                Transform br = Object.Instantiate(brick[xl.Value]);
                br.SetParent(panel);
                int x = xl.Key;
                br.SetPositionAndRotation(getPosition(x, y), Quaternion.identity);
            }
        }
        startTime = DateTime.Now;
        logger = (SkladLogger)skladWrapper.objects.First(x=>x is SkladLogger);
        asc = logger.logs.First();
    }
    void Update()
    {
        TimeSpan current = DateTime.Now - startTime;
        if (current > asc.lastUpdated) 
        {
            logger.logs.Remove(asc);
            if (asc.command == "Create AntBot")
            {
                Transform ab = Instantiate(AntBotTransform);
                ab.SetParent(panel);
                antsBot.Add(asc.uid, ab.GetComponent<AntBotUnity>());
                antsBot[asc.uid].antStateChange = asc;
                antsBot[asc.uid].SetPosition();
                antsBot[asc.uid].startTime = startTime;
            }
            else
            {
                antsBot[asc.uid].antStateChange = asc;
            }
            asc = logger.logs.First();
        }
    }
}