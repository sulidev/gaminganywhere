/*
* Copyright (c) 2015 Suliadi Marsetya
*
*
*/

#include <stdio.h>
#include <errno.h>
#include <list>
#include "ga-common.h"
#include "ga-conf.h"
#include "ga-module.h"
#include "adaptive-profile.h"
#include <fl/Headers.h>

fl::Engine* engine;

void profileMain()
{
	engine = new fl::Engine;
	engine->setName("Adaptive-stream");
}

void parseTerm(std::string tmp, std::string key, int type, fl::Variable* var)
{
	if(type==0)
		var = (fl::InputVariable*) var;
	else
		var = (fl::OutputVariable*) var;

	if(strcmp(ga_conf_mapreadv(tmp.c_str(),(key+"-type").c_str(),NULL,16),"trapezoid")==0)
	{
		var->addTerm(new fl::Trapezoid(ga_conf_mapreadv(tmp.c_str(),(key+"-name").c_str(),NULL,10),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point1").c_str()),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point2").c_str()),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point3").c_str()),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point4").c_str())));
	}
	else if(strcmp(ga_conf_mapreadv(tmp.c_str(),(key+"-type").c_str(),NULL,16),"triangle")==0)
	{
		var->addTerm(new fl::Triangle(ga_conf_mapreadv(tmp.c_str(),(key+"-name").c_str(),NULL,10),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point1").c_str()),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point2").c_str()),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point3").c_str())));
	}
}

bool parseConf()
{
	int inNum = ga_conf_readint("adaptive-input-num");
	int outNum = ga_conf_readint("adaptive-output-num");
	ga_error("adaptive-stream: input count = %d\n", inNum);
	ga_error("adaptive-stream: output count = %d\n", outNum);
	while(inNum>0)
	{
		fl::InputVariable* input = new fl::InputVariable;
		input->setEnabled = true;
		std::string tmp = "adaptive-input"+inNum;
		input->setName(ga_conf_mapreadv(tmp.c_str(),"name",NULL,10));
		input->setRange(0.000, 1.000);

		int termNum = ga_conf_mapreadint(tmp.c_str(),"term-num");
		while(termNum>0)
		{
			std::string key = "term" + termNum;
			parseTerm(tmp,key,0,input);
			termNum--;
		}
		engine->addInputVariable(input);
		inNum--;
	}

	while(outNum>0)
	{
		fl::OutputVariable* output = new fl::OutputVariable;
		output->setEnabled = true;
		std::string tmp = "adaptive-output"+outNum;
		output->setName(ga_conf_mapreadv(tmp.c_str(),"name",NULL,10));
		output->setRange(0.000, 1.000);

		int termNum = ga_conf_mapreadint(tmp.c_str(),"term-num");
		while(termNum>0)
		{
			std::string key = "term" + termNum;
			parseTerm(tmp,key,0,output);
			termNum--;
		}
		engine->addOutputVariable(output);

		fl::RuleBlock* rules = new fl::RuleBlock;
		rules->setEnabled = true;
		rules->setName("");
		rules->setConjunction(new fl::Minimum);
		rules->setDisjunction(new fl::Maximum);
		rules->setActivation(new fl::Minimum);

		int ruleNum = ga_conf_mapreadint(tmp.c_str(),"rule-num");
		while(ruleNum>0)
		{
			rules->addRule(fl::Rule::parse(ga_conf_mapreadv(tmp.c_str(),"rule"+ruleNum,NULL,128),engine));
		}
		outNum--;
	}

	return true;
}

ga_ioctl_reconfigure_t createParam(adaptiveProfile ap)
{
	ga_ioctl_reconfigure_t params;
	params.id = 0;
	params.bitrateKbps = ap.bitrate;
	params.bufsize = ap.vbv;
	params.crf = ap.crf;
	params.framerate_n = ap.fps;
	params.framerate_d = 1;
	return params;
}

ga_ioctl_reconfigure_t selectProfile()
{

	return createParam();
}