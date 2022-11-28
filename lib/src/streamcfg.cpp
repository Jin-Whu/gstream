#include <cctype>
#include <cstring>
#include <algorithm>
#include <functional>
#include <sstream>
#include <pugixml.hpp>

#include "streamlog.h"
#include "streamcfg.h"

namespace stream
{
	bool StreamCfg::load(const std::string &fn, SystemInfo &info)
	{
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(fn.c_str(), pugi::parse_default | pugi::parse_trim_pcdata);
		if (!result)
		{
			return false;
		}

		{
			std::string txt_flag = doc.child("system").child("log").child_value("txt");
			std::string stdout_flag = doc.child("system").child("log").child_value("stdout");
			std::string bin_flag = doc.child("system").child("log").child_value("bin");

			unsigned flag = 0, logflag = 0;

			flag = std::stoi(stdout_flag);
			if (flag)
				logflag |= StreamLog::STDOUT;

			flag = std::stoi(txt_flag);
			if (flag)
				logflag |= StreamLog::TXT;

			flag = std::stoi(bin_flag);
			if (flag)
				logflag |= StreamLog::BIN;

			StreamLog::set_flag(logflag);
		}

        auto service_nodes = doc.child("system").children("service");
		for (auto service_node = service_nodes.begin();  service_node != service_nodes.end(); ++service_node)
		{
			ServiceInfo serviceinfo;
			std::string service_id = service_node->child_value("id");
			serviceinfo.id = "SERVICE_" + service_id;

            auto manager_nodes = service_node->children("manager");
			for (auto manager_node = manager_nodes.begin(); manager_node != manager_nodes.end(); ++manager_node)
			{
				ManagerInfo managerinfo;
				std::string manager_id = manager_node->child_value("id");
				managerinfo.id = "MANAGER_" + service_id + "_" + manager_id;
				auto rovers = manager_node->children("rover");
				for (auto rover_node = rovers.begin(); rover_node != rovers.end(); ++rover_node)
				{
					std::string rover = rover_node->child_value();
					StreamInfo streaminfo;
					if (!parse(rover, streaminfo))
						continue;
					streaminfo.id = "STREAM_" + service_id + "_" + manager_id + "_" + streaminfo.mnt;
					managerinfo.rovers.push_back(streaminfo);
				}
				serviceinfo.managers.push_back(managerinfo);
			}
			info.services.push_back(serviceinfo);
		}
		return true;
	}

	/// parse string to streaminfo
	/// format: //user:password@addr:port/mnt decoder lat lon alt nmea version
	bool StreamCfg::parse(const std::string &s, StreamInfo &info)
	{
		std::string ss = s;
		if (ss.substr(0, 2) != "//")
			return false;
		const char *buff = ss.c_str() + 2;
		const char *p, *q;
		std::string decoder;
		int version;

		if ((p = std::strchr(buff, '/')))
		{
			std::string str(p + 1);
			std::istringstream iss(str);
			iss >> info.mnt >> decoder >> info.lat >> info.lon >> info.alt >> info.nmea >> version;
		}

		if ((p = std::strchr(buff, '@')))
		{
			if ((q = std::strchr(buff, ':')))
			{
				info.user = std::string(buff, q);
				info.pwd = std::string(q + 1, p);
			}
			++p;
		}
		else
			p = buff;

		if ((q = std::strchr(p, ':')))
		{
			info.host = std::string(p, q);
			if ((p = std::strchr(q, '/')))
			{
				info.port = std::stoi(std::string(q + 1, p));
			}
		}

		if (!std::strncmp(decoder.c_str(), "RTCM_3", std::strlen("RTCM_3")))
		{
			info.encode = StreamInfo::StreamEncode::TYPE_RTCM3;
		}
		else if (!std::strncmp(decoder.c_str(), "RTCM_2", std::strlen("RTCM_2")))
		{
			info.encode = StreamInfo::StreamEncode::TYPE_RTCM2;
		}
		else
		{
			info.encode = StreamInfo::StreamEncode::TYPE_NONE;
		}

		if (version == 1)
		{
			info.stype = StreamInfo::StreamType::TYPE_NTRIP;
		}
		else if (version == 2)
		{
			info.stype = StreamInfo::StreamType::TYPE_TCP;
		}
		else if (version == 3)
		{
			info.stype = StreamInfo::StreamType::TYPE_UDP;
		}
		else
		{
			info.stype = StreamInfo::StreamType::TYPE_NONE;
		}
		return true;
	}
} // namespace stream