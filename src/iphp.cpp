#include <windows.h>
#include "InitState.h"

#pragma pack(1)


HINSTANCE hLThis = 0;
HINSTANCE hL = 0;
FARPROC p[267] = { 0 };

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{

	if (reason == DLL_PROCESS_ATTACH)
	{
		hLThis = hInst;

		char bufd[MAX_PATH];
		GetSystemDirectory(bufd, MAX_PATH);
		strcat_s(bufd, "\\IPHLPAPI.dll");

		hL = LoadLibrary(bufd);
		if (!hL) return false;


		p[0] = GetProcAddress(hL, "AddIPAddress");
		p[1] = GetProcAddress(hL, "AllocateAndGetInterfaceInfoFromStack");
		p[2] = GetProcAddress(hL, "AllocateAndGetIpAddrTableFromStack");
		p[3] = GetProcAddress(hL, "CancelIPChangeNotify");
		p[4] = GetProcAddress(hL, "CancelMibChangeNotify2");
		p[5] = GetProcAddress(hL, "CloseCompartment");
		p[6] = GetProcAddress(hL, "CloseGetIPPhysicalInterfaceForDestination");
		p[7] = GetProcAddress(hL, "ConvertCompartmentGuidToId");
		p[8] = GetProcAddress(hL, "ConvertCompartmentIdToGuid");
		p[9] = GetProcAddress(hL, "ConvertGuidToStringA");
		p[10] = GetProcAddress(hL, "ConvertGuidToStringW");
		p[11] = GetProcAddress(hL, "ConvertInterfaceAliasToLuid");
		p[12] = GetProcAddress(hL, "ConvertInterfaceGuidToLuid");
		p[13] = GetProcAddress(hL, "ConvertInterfaceIndexToLuid");
		p[14] = GetProcAddress(hL, "ConvertInterfaceLuidToAlias");
		p[15] = GetProcAddress(hL, "ConvertInterfaceLuidToGuid");
		p[16] = GetProcAddress(hL, "ConvertInterfaceLuidToIndex");
		p[17] = GetProcAddress(hL, "ConvertInterfaceLuidToNameA");
		p[18] = GetProcAddress(hL, "ConvertInterfaceLuidToNameW");
		p[19] = GetProcAddress(hL, "ConvertInterfaceNameToLuidA");
		p[20] = GetProcAddress(hL, "ConvertInterfaceNameToLuidW");
		p[21] = GetProcAddress(hL, "ConvertInterfacePhysicalAddressToLuid");
		p[22] = GetProcAddress(hL, "ConvertIpv4MaskToLength");
		p[23] = GetProcAddress(hL, "ConvertLengthToIpv4Mask");
		p[24] = GetProcAddress(hL, "ConvertRemoteInterfaceAliasToLuid");
		p[25] = GetProcAddress(hL, "ConvertRemoteInterfaceGuidToLuid");
		p[26] = GetProcAddress(hL, "ConvertRemoteInterfaceIndexToLuid");
		p[27] = GetProcAddress(hL, "ConvertRemoteInterfaceLuidToAlias");
		p[28] = GetProcAddress(hL, "ConvertRemoteInterfaceLuidToGuid");
		p[29] = GetProcAddress(hL, "ConvertRemoteInterfaceLuidToIndex");
		p[30] = GetProcAddress(hL, "ConvertStringToGuidA");
		p[31] = GetProcAddress(hL, "ConvertStringToGuidW");
		p[32] = GetProcAddress(hL, "ConvertStringToInterfacePhysicalAddress");
		p[33] = GetProcAddress(hL, "CreateAnycastIpAddressEntry");
		p[34] = GetProcAddress(hL, "CreateIpForwardEntry");
		p[35] = GetProcAddress(hL, "CreateIpForwardEntry2");
		p[36] = GetProcAddress(hL, "CreateIpNetEntry");
		p[37] = GetProcAddress(hL, "CreateIpNetEntry2");
		p[38] = GetProcAddress(hL, "CreatePersistentTcpPortReservation");
		p[39] = GetProcAddress(hL, "CreatePersistentUdpPortReservation");
		p[40] = GetProcAddress(hL, "CreateProxyArpEntry");
		p[41] = GetProcAddress(hL, "CreateSortedAddressPairs");
		p[42] = GetProcAddress(hL, "CreateUnicastIpAddressEntry");
		p[43] = GetProcAddress(hL, "DeleteAnycastIpAddressEntry");
		p[44] = GetProcAddress(hL, "DeleteIPAddress");
		p[45] = GetProcAddress(hL, "DeleteIpForwardEntry");
		p[46] = GetProcAddress(hL, "DeleteIpForwardEntry2");
		p[47] = GetProcAddress(hL, "DeleteIpNetEntry");
		p[48] = GetProcAddress(hL, "DeleteIpNetEntry2");
		p[49] = GetProcAddress(hL, "DeletePersistentTcpPortReservation");
		p[50] = GetProcAddress(hL, "DeletePersistentUdpPortReservation");
		p[51] = GetProcAddress(hL, "DeleteProxyArpEntry");
		p[52] = GetProcAddress(hL, "DeleteUnicastIpAddressEntry");
		p[53] = GetProcAddress(hL, "DisableMediaSense");
		p[54] = GetProcAddress(hL, "EnableRouter");
		p[55] = GetProcAddress(hL, "FlushIpNetTable");
		p[56] = GetProcAddress(hL, "FlushIpNetTable2");
		p[57] = GetProcAddress(hL, "FlushIpPathTable");
		p[58] = GetProcAddress(hL, "FreeMibTable");
		p[59] = GetProcAddress(hL, "GetAdapterIndex");
		p[60] = GetProcAddress(hL, "GetAdapterOrderMap");
		p[61] = GetProcAddress(hL, "GetAdaptersAddresses");
		p[62] = GetProcAddress(hL, "GetAdaptersInfo");
		p[63] = GetProcAddress(hL, "GetAnycastIpAddressEntry");
		p[64] = GetProcAddress(hL, "GetAnycastIpAddressTable");
		p[65] = GetProcAddress(hL, "GetBestInterface");
		p[66] = GetProcAddress(hL, "GetBestInterfaceEx");
		p[67] = GetProcAddress(hL, "GetBestRoute");
		p[68] = GetProcAddress(hL, "GetBestRoute2");
		p[69] = GetProcAddress(hL, "GetCurrentThreadCompartmentId");
		p[70] = GetProcAddress(hL, "GetExtendedTcpTable");
		p[71] = GetProcAddress(hL, "GetExtendedUdpTable");
		p[72] = GetProcAddress(hL, "GetFriendlyIfIndex");
		p[73] = GetProcAddress(hL, "GetIcmpStatistics");
		p[74] = GetProcAddress(hL, "GetIcmpStatisticsEx");
		p[75] = GetProcAddress(hL, "GetIfEntry");
		p[76] = GetProcAddress(hL, "GetIfEntry2");
		p[77] = GetProcAddress(hL, "GetIfStackTable");
		p[78] = GetProcAddress(hL, "GetIfTable");
		p[79] = GetProcAddress(hL, "GetIfTable2");
		p[80] = GetProcAddress(hL, "GetIfTable2Ex");
		p[81] = GetProcAddress(hL, "GetInterfaceInfo");
		p[82] = GetProcAddress(hL, "GetInvertedIfStackTable");
		p[83] = GetProcAddress(hL, "GetIpAddrTable");
		p[84] = GetProcAddress(hL, "GetIpErrorString");
		p[85] = GetProcAddress(hL, "GetIpForwardEntry2");
		p[86] = GetProcAddress(hL, "GetIpForwardTable");
		p[87] = GetProcAddress(hL, "GetIpForwardTable2");
		p[88] = GetProcAddress(hL, "GetIpInterfaceEntry");
		p[89] = GetProcAddress(hL, "GetIpInterfaceTable");
		p[90] = GetProcAddress(hL, "GetIpNetEntry2");
		p[91] = GetProcAddress(hL, "GetIpNetTable");
		p[92] = GetProcAddress(hL, "GetIpNetTable2");
		p[93] = GetProcAddress(hL, "GetIpNetworkConnectionBandwidthEstimates");
		p[94] = GetProcAddress(hL, "GetIpPathEntry");
		p[95] = GetProcAddress(hL, "GetIpPathTable");
		p[96] = GetProcAddress(hL, "GetIpStatistics");
		p[97] = GetProcAddress(hL, "GetIpStatisticsEx");
		p[98] = GetProcAddress(hL, "GetMulticastIpAddressEntry");
		p[99] = GetProcAddress(hL, "GetMulticastIpAddressTable");
		p[100] = GetProcAddress(hL, "GetNetworkInformation");
		p[101] = GetProcAddress(hL, "GetNetworkParams");
		p[102] = GetProcAddress(hL, "GetNumberOfInterfaces");
		p[103] = GetProcAddress(hL, "GetOwnerModuleFromPidAndInfo");
		p[104] = GetProcAddress(hL, "GetOwnerModuleFromTcp6Entry");
		p[105] = GetProcAddress(hL, "GetOwnerModuleFromTcpEntry");
		p[106] = GetProcAddress(hL, "GetOwnerModuleFromUdp6Entry");
		p[107] = GetProcAddress(hL, "GetOwnerModuleFromUdpEntry");
		p[108] = GetProcAddress(hL, "GetPerAdapterInfo");
		p[109] = GetProcAddress(hL, "GetPerTcp6ConnectionEStats");
		p[110] = GetProcAddress(hL, "GetPerTcp6ConnectionStats");
		p[111] = GetProcAddress(hL, "GetPerTcpConnectionEStats");
		p[112] = GetProcAddress(hL, "GetPerTcpConnectionStats");
		p[113] = GetProcAddress(hL, "GetRTTAndHopCount");
		p[114] = GetProcAddress(hL, "GetSessionCompartmentId");
		p[115] = GetProcAddress(hL, "GetTcp6Table");
		p[116] = GetProcAddress(hL, "GetTcp6Table2");
		p[117] = GetProcAddress(hL, "GetTcpStatistics");
		p[118] = GetProcAddress(hL, "GetTcpStatisticsEx");
		p[119] = GetProcAddress(hL, "GetTcpTable");
		p[120] = GetProcAddress(hL, "GetTcpTable2");
		p[121] = GetProcAddress(hL, "GetTeredoPort");
		p[122] = GetProcAddress(hL, "GetUdp6Table");
		p[123] = GetProcAddress(hL, "GetUdpStatistics");
		p[124] = GetProcAddress(hL, "GetUdpStatisticsEx");
		p[125] = GetProcAddress(hL, "GetUdpTable");
		p[126] = GetProcAddress(hL, "GetUniDirectionalAdapterInfo");
		p[127] = GetProcAddress(hL, "GetUnicastIpAddressEntry");
		p[128] = GetProcAddress(hL, "GetUnicastIpAddressTable");
		p[129] = GetProcAddress(hL, "Icmp6CreateFile");
		p[130] = GetProcAddress(hL, "Icmp6ParseReplies");
		p[131] = GetProcAddress(hL, "Icmp6SendEcho2");
		p[132] = GetProcAddress(hL, "IcmpCloseHandle");
		p[133] = GetProcAddress(hL, "IcmpCreateFile");
		p[134] = GetProcAddress(hL, "IcmpParseReplies");
		p[135] = GetProcAddress(hL, "IcmpSendEcho");
		p[136] = GetProcAddress(hL, "IcmpSendEcho2");
		p[137] = GetProcAddress(hL, "IcmpSendEcho2Ex");
		p[138] = GetProcAddress(hL, "InitializeIpForwardEntry");
		p[139] = GetProcAddress(hL, "InitializeIpInterfaceEntry");
		p[140] = GetProcAddress(hL, "InitializeUnicastIpAddressEntry");
		p[141] = GetProcAddress(hL, "InternalCleanupPersistentStore");
		p[142] = GetProcAddress(hL, "InternalCreateAnycastIpAddressEntry");
		p[143] = GetProcAddress(hL, "InternalCreateIpForwardEntry");
		p[144] = GetProcAddress(hL, "InternalCreateIpForwardEntry2");
		p[145] = GetProcAddress(hL, "InternalCreateIpNetEntry");
		p[146] = GetProcAddress(hL, "InternalCreateIpNetEntry2");
		p[147] = GetProcAddress(hL, "InternalCreateUnicastIpAddressEntry");
		p[148] = GetProcAddress(hL, "InternalDeleteAnycastIpAddressEntry");
		p[149] = GetProcAddress(hL, "InternalDeleteIpForwardEntry");
		p[150] = GetProcAddress(hL, "InternalDeleteIpForwardEntry2");
		p[151] = GetProcAddress(hL, "InternalDeleteIpNetEntry");
		p[152] = GetProcAddress(hL, "InternalDeleteIpNetEntry2");
		p[153] = GetProcAddress(hL, "InternalDeleteUnicastIpAddressEntry");
		p[154] = GetProcAddress(hL, "InternalFindInterfaceByAddress");
		p[155] = GetProcAddress(hL, "InternalGetAnycastIpAddressEntry");
		p[156] = GetProcAddress(hL, "InternalGetAnycastIpAddressTable");
		p[157] = GetProcAddress(hL, "InternalGetForwardIpTable2");
		p[158] = GetProcAddress(hL, "InternalGetIPPhysicalInterfaceForDestination");
		p[159] = GetProcAddress(hL, "InternalGetIfEntry2");
		p[160] = GetProcAddress(hL, "InternalGetIfTable");
		p[161] = GetProcAddress(hL, "InternalGetIfTable2");
		p[162] = GetProcAddress(hL, "InternalGetIpAddrTable");
		p[163] = GetProcAddress(hL, "InternalGetIpForwardEntry2");
		p[164] = GetProcAddress(hL, "InternalGetIpForwardTable");
		p[165] = GetProcAddress(hL, "InternalGetIpInterfaceEntry");
		p[166] = GetProcAddress(hL, "InternalGetIpInterfaceTable");
		p[167] = GetProcAddress(hL, "InternalGetIpNetEntry2");
		p[168] = GetProcAddress(hL, "InternalGetIpNetTable");
		p[169] = GetProcAddress(hL, "InternalGetIpNetTable2");
		p[170] = GetProcAddress(hL, "InternalGetMulticastIpAddressEntry");
		p[171] = GetProcAddress(hL, "InternalGetMulticastIpAddressTable");
		p[172] = GetProcAddress(hL, "InternalGetRtcSlotInformation");
		p[173] = GetProcAddress(hL, "InternalGetTcp6Table2");
		p[174] = GetProcAddress(hL, "InternalGetTcp6TableWithOwnerModule");
		p[175] = GetProcAddress(hL, "InternalGetTcp6TableWithOwnerPid");
		p[176] = GetProcAddress(hL, "InternalGetTcpTable");
		p[177] = GetProcAddress(hL, "InternalGetTcpTable2");
		p[178] = GetProcAddress(hL, "InternalGetTcpTableEx");
		p[179] = GetProcAddress(hL, "InternalGetTcpTableWithOwnerModule");
		p[180] = GetProcAddress(hL, "InternalGetTcpTableWithOwnerPid");
		p[181] = GetProcAddress(hL, "InternalGetTunnelPhysicalAdapter");
		p[182] = GetProcAddress(hL, "InternalGetUdp6TableWithOwnerModule");
		p[183] = GetProcAddress(hL, "InternalGetUdp6TableWithOwnerPid");
		p[184] = GetProcAddress(hL, "InternalGetUdpTable");
		p[185] = GetProcAddress(hL, "InternalGetUdpTableEx");
		p[186] = GetProcAddress(hL, "InternalGetUdpTableWithOwnerModule");
		p[187] = GetProcAddress(hL, "InternalGetUdpTableWithOwnerPid");
		p[188] = GetProcAddress(hL, "InternalGetUnicastIpAddressEntry");
		p[189] = GetProcAddress(hL, "InternalGetUnicastIpAddressTable");
		p[190] = GetProcAddress(hL, "InternalIcmpCreateFileEx");
		p[191] = GetProcAddress(hL, "InternalSetIfEntry");
		p[192] = GetProcAddress(hL, "InternalSetIpForwardEntry");
		p[193] = GetProcAddress(hL, "InternalSetIpForwardEntry2");
		p[194] = GetProcAddress(hL, "InternalSetIpInterfaceEntry");
		p[195] = GetProcAddress(hL, "InternalSetIpNetEntry");
		p[196] = GetProcAddress(hL, "InternalSetIpNetEntry2");
		p[197] = GetProcAddress(hL, "InternalSetIpStats");
		p[198] = GetProcAddress(hL, "InternalSetTcpEntry");
		p[199] = GetProcAddress(hL, "InternalSetTeredoPort");
		p[200] = GetProcAddress(hL, "InternalSetUnicastIpAddressEntry");
		p[201] = GetProcAddress(hL, "IpReleaseAddress");
		p[202] = GetProcAddress(hL, "IpRenewAddress");
		p[203] = GetProcAddress(hL, "LookupPersistentTcpPortReservation");
		p[204] = GetProcAddress(hL, "LookupPersistentUdpPortReservation");
		p[205] = GetProcAddress(hL, "NTPTimeToNTFileTime");
		p[206] = GetProcAddress(hL, "NTTimeToNTPTime");
		p[207] = GetProcAddress(hL, "NhGetGuidFromInterfaceName");
		p[208] = GetProcAddress(hL, "NhGetInterfaceDescriptionFromGuid");
		p[209] = GetProcAddress(hL, "NhGetInterfaceNameFromDeviceGuid");
		p[210] = GetProcAddress(hL, "NhGetInterfaceNameFromGuid");
		p[211] = GetProcAddress(hL, "NhpAllocateAndGetInterfaceInfoFromStack");
		p[212] = GetProcAddress(hL, "NotifyAddrChange");
		p[213] = GetProcAddress(hL, "NotifyCompartmentChange");
		p[214] = GetProcAddress(hL, "NotifyIpInterfaceChange");
		p[215] = GetProcAddress(hL, "NotifyRouteChange");
		p[216] = GetProcAddress(hL, "NotifyRouteChange2");
		p[217] = GetProcAddress(hL, "NotifyStableUnicastIpAddressTable");
		p[218] = GetProcAddress(hL, "NotifyTeredoPortChange");
		p[219] = GetProcAddress(hL, "NotifyUnicastIpAddressChange");
		p[220] = GetProcAddress(hL, "OpenCompartment");
		p[221] = GetProcAddress(hL, "ParseNetworkString");
		p[222] = GetProcAddress(hL, "ResolveIpNetEntry2");
		p[223] = GetProcAddress(hL, "ResolveNeighbor");
		p[224] = GetProcAddress(hL, "RestoreMediaSense");
		p[225] = GetProcAddress(hL, "SendARP");
		p[226] = GetProcAddress(hL, "SetAdapterIpAddress");
		p[227] = GetProcAddress(hL, "SetCurrentThreadCompartmentId");
		p[228] = GetProcAddress(hL, "SetIfEntry");
		p[229] = GetProcAddress(hL, "SetIpForwardEntry");
		p[230] = GetProcAddress(hL, "SetIpForwardEntry2");
		p[231] = GetProcAddress(hL, "SetIpInterfaceEntry");
		p[232] = GetProcAddress(hL, "SetIpNetEntry");
		p[233] = GetProcAddress(hL, "SetIpNetEntry2");
		p[234] = GetProcAddress(hL, "SetIpStatistics");
		p[235] = GetProcAddress(hL, "SetIpStatisticsEx");
		p[236] = GetProcAddress(hL, "SetIpTTL");
		p[237] = GetProcAddress(hL, "SetNetworkInformation");
		p[238] = GetProcAddress(hL, "SetPerTcp6ConnectionEStats");
		p[239] = GetProcAddress(hL, "SetPerTcp6ConnectionStats");
		p[240] = GetProcAddress(hL, "SetPerTcpConnectionEStats");
		p[241] = GetProcAddress(hL, "SetPerTcpConnectionStats");
		p[242] = GetProcAddress(hL, "SetSessionCompartmentId");
		p[243] = GetProcAddress(hL, "SetTcpEntry");
		p[244] = GetProcAddress(hL, "SetUnicastIpAddressEntry");
		p[245] = GetProcAddress(hL, "UnenableRouter");
		p[246] = GetProcAddress(hL, "_PfAddFiltersToInterface@24");
		p[247] = GetProcAddress(hL, "_PfAddGlobalFilterToInterface@8");
		p[248] = GetProcAddress(hL, "_PfBindInterfaceToIPAddress@12");
		p[249] = GetProcAddress(hL, "_PfBindInterfaceToIndex@16");
		p[250] = GetProcAddress(hL, "_PfCreateInterface@24");
		p[251] = GetProcAddress(hL, "_PfDeleteInterface@4");
		p[252] = GetProcAddress(hL, "_PfDeleteLog@0");
		p[253] = GetProcAddress(hL, "_PfGetInterfaceStatistics@16");
		p[254] = GetProcAddress(hL, "_PfMakeLog@4");
		p[255] = GetProcAddress(hL, "_PfRebindFilters@8");
		p[256] = GetProcAddress(hL, "_PfRemoveFilterHandles@12");
		p[257] = GetProcAddress(hL, "_PfRemoveFiltersFromInterface@20");
		p[258] = GetProcAddress(hL, "_PfRemoveGlobalFilterFromInterface@8");
		p[259] = GetProcAddress(hL, "_PfSetLogBuffer@28");
		p[260] = GetProcAddress(hL, "_PfTestPacket@20");
		p[261] = GetProcAddress(hL, "_PfUnBindInterface@4");
		p[262] = GetProcAddress(hL, "do_echo_rep");
		p[263] = GetProcAddress(hL, "do_echo_req");
		p[264] = GetProcAddress(hL, "if_indextoname");
		p[265] = GetProcAddress(hL, "if_nametoindex");
		p[266] = GetProcAddress(hL, "register_icmp");

		pd2hook::InitiateStates();

	}
	if (reason == DLL_PROCESS_DETACH)
	{
		pd2hook::DestroyStates();
		FreeLibrary(hL);
	}

	return 1;
}

// TODO: x64 compatibility
// https://stackoverflow.com/questions/26637755/how-to-do-a-naked-function-and-inline-assembler-in-x64-visual-c
// http://archives.miloush.net/michkap/archive/2005/05/11/416624.html
// https://stackoverflow.com/questions/11385369/c-use-naked-function
// http://mariusbancila.ro/blog/2010/10/17/no-more-inline-asm-in-vc-on-x64/
// https://stackoverflow.com/questions/27218869/c-proxy-dll-64-bit
// https://www.codeproject.com/Articles/16541/Create-your-Proxy-DLLs-automatically
// https://www.youtube.com/watch?v=lAY_ww8SNmM
// https://github.com/Erik-JS/proxy-dll-64

// FIXME: THIS STUFF ACTUALLY WORKS FINE SOMEHOW AND DON'T ASK ME HOW BECAUSE I MYSELF HAVE NO IDEA EITHER, BUT FIXME ANYWAY!


// AddIPAddress
extern "C" void __stdcall __E__0__()
{
	return;
}

// AllocateAndGetInterfaceInfoFromStack
extern "C" void __stdcall __E__1__()
{
	return;
}

// AllocateAndGetIpAddrTableFromStack
extern "C" void __stdcall __E__2__()
{
	return;
}

// CancelIPChangeNotify
extern "C" void __stdcall __E__3__()
{
	return;
}

// CancelMibChangeNotify2
extern "C" void __stdcall __E__4__()
{
	return;
}

// CloseCompartment
extern "C" void __stdcall __E__5__()
{
	return;
}

// CloseGetIPPhysicalInterfaceForDestination
extern "C" void __stdcall __E__6__()
{
	return;
}

// ConvertCompartmentGuidToId
extern "C" void __stdcall __E__7__()
{
	return;
}

// ConvertCompartmentIdToGuid
extern "C" void __stdcall __E__8__()
{
	return;
}

// ConvertGuidToStringA
extern "C" void __stdcall __E__9__()
{
	return;
}

// ConvertGuidToStringW
extern "C" void __stdcall __E__10__()
{
	return;
}

// ConvertInterfaceAliasToLuid
extern "C" void __stdcall __E__11__()
{
	return;
}

// ConvertInterfaceGuidToLuid
extern "C" void __stdcall __E__12__()
{
	return;
}

// ConvertInterfaceIndexToLuid
extern "C" void __stdcall __E__13__()
{
	return;
}

// ConvertInterfaceLuidToAlias
extern "C" void __stdcall __E__14__()
{
	return;
}

// ConvertInterfaceLuidToGuid
extern "C" void __stdcall __E__15__()
{
	return;
}

// ConvertInterfaceLuidToIndex
extern "C" void __stdcall __E__16__()
{
	return;
}

// ConvertInterfaceLuidToNameA
extern "C" void __stdcall __E__17__()
{
	return;
}

// ConvertInterfaceLuidToNameW
extern "C" void __stdcall __E__18__()
{
	return;
}

// ConvertInterfaceNameToLuidA
extern "C" void __stdcall __E__19__()
{
	return;
}

// ConvertInterfaceNameToLuidW
extern "C" void __stdcall __E__20__()
{
	return;
}

// ConvertInterfacePhysicalAddressToLuid
extern "C" void __stdcall __E__21__()
{
	return;
}

// ConvertIpv4MaskToLength
extern "C" void __stdcall __E__22__()
{
	return;
}

// ConvertLengthToIpv4Mask
extern "C" void __stdcall __E__23__()
{
	return;
}

// ConvertRemoteInterfaceAliasToLuid
extern "C" void __stdcall __E__24__()
{
	return;
}

// ConvertRemoteInterfaceGuidToLuid
extern "C" void __stdcall __E__25__()
{
	return;
}

// ConvertRemoteInterfaceIndexToLuid
extern "C" void __stdcall __E__26__()
{
	return;
}

// ConvertRemoteInterfaceLuidToAlias
extern "C" void __stdcall __E__27__()
{
	return;
}

// ConvertRemoteInterfaceLuidToGuid
extern "C" void __stdcall __E__28__()
{
	return;
}

// ConvertRemoteInterfaceLuidToIndex
extern "C" void __stdcall __E__29__()
{
	return;
}

// ConvertStringToGuidA
extern "C" void __stdcall __E__30__()
{
	return;
}

// ConvertStringToGuidW
extern "C" void __stdcall __E__31__()
{
	return;
}

// ConvertStringToInterfacePhysicalAddress
extern "C" void __stdcall __E__32__()
{
	return;
}

// CreateAnycastIpAddressEntry
extern "C" void __stdcall __E__33__()
{
	return;
}

// CreateIpForwardEntry
extern "C" void __stdcall __E__34__()
{
	return;
}

// CreateIpForwardEntry2
extern "C" void __stdcall __E__35__()
{
	return;
}

// CreateIpNetEntry
extern "C" void __stdcall __E__36__()
{
	return;
}

// CreateIpNetEntry2
extern "C" void __stdcall __E__37__()
{
	return;
}

// CreatePersistentTcpPortReservation
extern "C" void __stdcall __E__38__()
{
	return;
}

// CreatePersistentUdpPortReservation
extern "C" void __stdcall __E__39__()
{
	return;
}

// CreateProxyArpEntry
extern "C" void __stdcall __E__40__()
{
	return;
}

// CreateSortedAddressPairs
extern "C" void __stdcall __E__41__()
{
	return;
}

// CreateUnicastIpAddressEntry
extern "C" void __stdcall __E__42__()
{
	return;
}

// DeleteAnycastIpAddressEntry
extern "C" void __stdcall __E__43__()
{
	return;
}

// DeleteIPAddress
extern "C" void __stdcall __E__44__()
{
	return;
}

// DeleteIpForwardEntry
extern "C" void __stdcall __E__45__()
{
	return;
}

// DeleteIpForwardEntry2
extern "C" void __stdcall __E__46__()
{
	return;
}

// DeleteIpNetEntry
extern "C" void __stdcall __E__47__()
{
	return;
}

// DeleteIpNetEntry2
extern "C" void __stdcall __E__48__()
{
	return;
}

// DeletePersistentTcpPortReservation
extern "C" void __stdcall __E__49__()
{
	return;
}

// DeletePersistentUdpPortReservation
extern "C" void __stdcall __E__50__()
{
	return;
}

// DeleteProxyArpEntry
extern "C" void __stdcall __E__51__()
{
	return;
}

// DeleteUnicastIpAddressEntry
extern "C" void __stdcall __E__52__()
{
	return;
}

// DisableMediaSense
extern "C" void __stdcall __E__53__()
{
	return;
}

// EnableRouter
extern "C" void __stdcall __E__54__()
{
	return;
}

// FlushIpNetTable
extern "C" void __stdcall __E__55__()
{
	return;
}

// FlushIpNetTable2
extern "C" void __stdcall __E__56__()
{
	return;
}

// FlushIpPathTable
extern "C" void __stdcall __E__57__()
{
	return;
}

// FreeMibTable
extern "C" void __stdcall __E__58__()
{
	return;
}

// GetAdapterIndex
extern "C" void __stdcall __E__59__()
{
	return;
}

// GetAdapterOrderMap
extern "C" void __stdcall __E__60__()
{
	return;
}

// GetAdaptersAddresses
extern "C" void __stdcall __E__61__()
{
	return;
}

// GetAdaptersInfo
extern "C" /*__declspec(naked)*/ DWORD __stdcall __E__62__(void* pAdapterInfo, void* pOutBufLen)
{
	typedef DWORD(__stdcall *pS)(void*, void*);
	pS pps = (pS)p[0 * 4];
	return pps(pAdapterInfo, pOutBufLen);
}

// GetAnycastIpAddressEntry
extern "C" void __stdcall __E__63__()
{
	return;
}

// GetAnycastIpAddressTable
extern "C" void __stdcall __E__64__()
{
	return;
}

// GetBestInterface
extern "C" void __stdcall __E__65__()
{
	return;
}

// GetBestInterfaceEx
extern "C" void __stdcall __E__66__()
{
	return;
}

// GetBestRoute
extern "C" void __stdcall __E__67__()
{
	return;
}

// GetBestRoute2
extern "C" void __stdcall __E__68__()
{
	return;
}

// GetCurrentThreadCompartmentId
extern "C" void __stdcall __E__69__()
{
	return;
}

// GetExtendedTcpTable
extern "C" void __stdcall __E__70__()
{
	return;
}

// GetExtendedUdpTable
extern "C" void __stdcall __E__71__()
{
	return;
}

// GetFriendlyIfIndex
extern "C" void __stdcall __E__72__()
{
	return;
}

// GetIcmpStatistics
extern "C" void __stdcall __E__73__()
{
	return;
}

// GetIcmpStatisticsEx
extern "C" void __stdcall __E__74__()
{
	return;
}

// GetIfEntry
extern "C" void __stdcall __E__75__()
{
	return;
}

// GetIfEntry2
extern "C" void __stdcall __E__76__()
{
	return;
}

// GetIfStackTable
extern "C" void __stdcall __E__77__()
{
	return;
}

// GetIfTable
extern "C" void __stdcall __E__78__()
{
	return;
}

// GetIfTable2
extern "C" void __stdcall __E__79__()
{
	return;
}

// GetIfTable2Ex
extern "C" void __stdcall __E__80__()
{
	return;
}

// GetInterfaceInfo
extern "C" void __stdcall __E__81__()
{
	return;
}

// GetInvertedIfStackTable
extern "C" void __stdcall __E__82__()
{
	return;
}

// GetIpAddrTable
extern "C" /*__declspec(naked)*/ DWORD __stdcall __E__83__(void* pIpAddrTable, void* pdwSize, BOOL bOrder)
{
	typedef DWORD(__stdcall *pS)(void*, void*, BOOL);
	pS pps = (pS)p[1 * 4];
	return pps(pIpAddrTable, pdwSize, bOrder);
}

// GetIpErrorString
extern "C" void __stdcall __E__84__()
{
	return;
}

// GetIpForwardEntry2
extern "C" void __stdcall __E__85__()
{
	return;
}

// GetIpForwardTable
extern "C" void __stdcall __E__86__()
{
	return;
}

// GetIpForwardTable2
extern "C" void __stdcall __E__87__()
{
	return;
}

// GetIpInterfaceEntry
extern "C" void __stdcall __E__88__()
{
	return;
}

// GetIpInterfaceTable
extern "C" void __stdcall __E__89__()
{
	return;
}

// GetIpNetEntry2
extern "C" void __stdcall __E__90__()
{
	return;
}

// GetIpNetTable
extern "C" void __stdcall __E__91__()
{
	return;
}

// GetIpNetTable2
extern "C" void __stdcall __E__92__()
{
	return;
}

// GetIpNetworkConnectionBandwidthEstimates
extern "C" void __stdcall __E__93__()
{
	return;
}

// GetIpPathEntry
extern "C" void __stdcall __E__94__()
{
	return;
}

// GetIpPathTable
extern "C" void __stdcall __E__95__()
{
	return;
}

// GetIpStatistics
extern "C" void __stdcall __E__96__()
{
	return;
}

// GetIpStatisticsEx
extern "C" void __stdcall __E__97__()
{
	return;
}

// GetMulticastIpAddressEntry
extern "C" void __stdcall __E__98__()
{
	return;
}

// GetMulticastIpAddressTable
extern "C" void __stdcall __E__99__()
{
	return;
}

// GetNetworkInformation
extern "C" void __stdcall __E__100__()
{
	return;
}

// GetNetworkParams
extern "C" void __stdcall __E__101__()
{
	return;
}

// GetNumberOfInterfaces
extern "C" void __stdcall __E__102__()
{
	return;
}

// GetOwnerModuleFromPidAndInfo
extern "C" void __stdcall __E__103__()
{
	return;
}

// GetOwnerModuleFromTcp6Entry
extern "C" void __stdcall __E__104__()
{
	return;
}

// GetOwnerModuleFromTcpEntry
extern "C" void __stdcall __E__105__()
{
	return;
}

// GetOwnerModuleFromUdp6Entry
extern "C" void __stdcall __E__106__()
{
	return;
}

// GetOwnerModuleFromUdpEntry
extern "C" void __stdcall __E__107__()
{
	return;
}

// GetPerAdapterInfo
extern "C" void __stdcall __E__108__()
{
	return;
}

// GetPerTcp6ConnectionEStats
extern "C" void __stdcall __E__109__()
{
	return;
}

// GetPerTcp6ConnectionStats
extern "C" void __stdcall __E__110__()
{
	return;
}

// GetPerTcpConnectionEStats
extern "C" void __stdcall __E__111__()
{
	return;
}

// GetPerTcpConnectionStats
extern "C" void __stdcall __E__112__()
{
	return;
}

// GetRTTAndHopCount
extern "C" void __stdcall __E__113__()
{
	return;
}

// GetSessionCompartmentId
extern "C" void __stdcall __E__114__()
{
	return;
}

// GetTcp6Table
extern "C" void __stdcall __E__115__()
{
	return;
}

// GetTcp6Table2
extern "C" void __stdcall __E__116__()
{
	return;
}

// GetTcpStatistics
extern "C" void __stdcall __E__117__()
{
	return;
}

// GetTcpStatisticsEx
extern "C" void __stdcall __E__118__()
{
	return;
}

// GetTcpTable
extern "C" void __stdcall __E__119__()
{
	return;
}

// GetTcpTable2
extern "C" void __stdcall __E__120__()
{
	return;
}

// GetTeredoPort
extern "C" void __stdcall __E__121__()
{
	return;
}

// GetUdp6Table
extern "C" void __stdcall __E__122__()
{
	return;
}

// GetUdpStatistics
extern "C" void __stdcall __E__123__()
{
	return;
}

// GetUdpStatisticsEx
extern "C" void __stdcall __E__124__()
{
	return;
}

// GetUdpTable
extern "C" void __stdcall __E__125__()
{
	return;
}

// GetUniDirectionalAdapterInfo
extern "C" void __stdcall __E__126__()
{
	return;
}

// GetUnicastIpAddressEntry
extern "C" void __stdcall __E__127__()
{
	return;
}

// GetUnicastIpAddressTable
extern "C" void __stdcall __E__128__()
{
	return;
}

// Icmp6CreateFile
extern "C" void __stdcall __E__129__()
{
	return;
}

// Icmp6ParseReplies
extern "C" void __stdcall __E__130__()
{
	return;
}

// Icmp6SendEcho2
extern "C" void __stdcall __E__131__()
{
	return;
}

// IcmpCloseHandle
extern "C" void __stdcall __E__132__()
{
	return;
}

// IcmpCreateFile
extern "C" void __stdcall __E__133__()
{
	return;
}

// IcmpParseReplies
extern "C" void __stdcall __E__134__()
{
	return;
}

// IcmpSendEcho
extern "C" void __stdcall __E__135__()
{
	return;
}

// IcmpSendEcho2
extern "C" void __stdcall __E__136__()
{
	return;
}

// IcmpSendEcho2Ex
extern "C" void __stdcall __E__137__()
{
	return;
}

// InitializeIpForwardEntry
extern "C" void __stdcall __E__138__()
{
	return;
}

// InitializeIpInterfaceEntry
extern "C" void __stdcall __E__139__()
{
	return;
}

// InitializeUnicastIpAddressEntry
extern "C" void __stdcall __E__140__()
{
	return;
}

// InternalCleanupPersistentStore
extern "C" void __stdcall __E__141__()
{
	return;
}

// InternalCreateAnycastIpAddressEntry
extern "C" void __stdcall __E__142__()
{
	return;
}

// InternalCreateIpForwardEntry
extern "C" void __stdcall __E__143__()
{
	return;
}

// InternalCreateIpForwardEntry2
extern "C" void __stdcall __E__144__()
{
	return;
}

// InternalCreateIpNetEntry
extern "C" void __stdcall __E__145__()
{
	return;
}

// InternalCreateIpNetEntry2
extern "C" void __stdcall __E__146__()
{
	return;
}

// InternalCreateUnicastIpAddressEntry
extern "C" void __stdcall __E__147__()
{
	return;
}

// InternalDeleteAnycastIpAddressEntry
extern "C" void __stdcall __E__148__()
{
	return;
}

// InternalDeleteIpForwardEntry
extern "C" void __stdcall __E__149__()
{
	return;
}

// InternalDeleteIpForwardEntry2
extern "C" void __stdcall __E__150__()
{
	return;
}

// InternalDeleteIpNetEntry
extern "C" void __stdcall __E__151__()
{
	return;
}

// InternalDeleteIpNetEntry2
extern "C" void __stdcall __E__152__()
{
	return;
}

// InternalDeleteUnicastIpAddressEntry
extern "C" void __stdcall __E__153__()
{
	return;
}

// InternalFindInterfaceByAddress
extern "C" void __stdcall __E__154__()
{
	return;
}

// InternalGetAnycastIpAddressEntry
extern "C" void __stdcall __E__155__()
{
	return;
}

// InternalGetAnycastIpAddressTable
extern "C" void __stdcall __E__156__()
{
	return;
}

// InternalGetForwardIpTable2
extern "C" void __stdcall __E__157__()
{
	return;
}

// InternalGetIPPhysicalInterfaceForDestination
extern "C" void __stdcall __E__158__()
{
	return;
}

// InternalGetIfEntry2
extern "C" void __stdcall __E__159__()
{
	return;
}

// InternalGetIfTable
extern "C" void __stdcall __E__160__()
{
	return;
}

// InternalGetIfTable2
extern "C" void __stdcall __E__161__()
{
	return;
}

// InternalGetIpAddrTable
extern "C" void __stdcall __E__162__()
{
	return;
}

// InternalGetIpForwardEntry2
extern "C" void __stdcall __E__163__()
{
	return;
}

// InternalGetIpForwardTable
extern "C" void __stdcall __E__164__()
{
	return;
}

// InternalGetIpInterfaceEntry
extern "C" void __stdcall __E__165__()
{
	return;
}

// InternalGetIpInterfaceTable
extern "C" void __stdcall __E__166__()
{
	return;
}

// InternalGetIpNetEntry2
extern "C" void __stdcall __E__167__()
{
	return;
}

// InternalGetIpNetTable
extern "C" void __stdcall __E__168__()
{
	return;
}

// InternalGetIpNetTable2
extern "C" void __stdcall __E__169__()
{
	return;
}

// InternalGetMulticastIpAddressEntry
extern "C" void __stdcall __E__170__()
{
	return;
}

// InternalGetMulticastIpAddressTable
extern "C" void __stdcall __E__171__()
{
	return;
}

// InternalGetRtcSlotInformation
extern "C" void __stdcall __E__172__()
{
	return;
}

// InternalGetTcp6Table2
extern "C" void __stdcall __E__173__()
{
	return;
}

// InternalGetTcp6TableWithOwnerModule
extern "C" void __stdcall __E__174__()
{
	return;
}

// InternalGetTcp6TableWithOwnerPid
extern "C" void __stdcall __E__175__()
{
	return;
}

// InternalGetTcpTable
extern "C" void __stdcall __E__176__()
{
	return;
}

// InternalGetTcpTable2
extern "C" void __stdcall __E__177__()
{
	return;
}

// InternalGetTcpTableEx
extern "C" void __stdcall __E__178__()
{
	return;
}

// InternalGetTcpTableWithOwnerModule
extern "C" void __stdcall __E__179__()
{
	return;
}

// InternalGetTcpTableWithOwnerPid
extern "C" void __stdcall __E__180__()
{
	return;
}

// InternalGetTunnelPhysicalAdapter
extern "C" void __stdcall __E__181__()
{
	return;
}

// InternalGetUdp6TableWithOwnerModule
extern "C" void __stdcall __E__182__()
{
	return;
}

// InternalGetUdp6TableWithOwnerPid
extern "C" void __stdcall __E__183__()
{
	return;
}

// InternalGetUdpTable
extern "C" void __stdcall __E__184__()
{
	return;
}

// InternalGetUdpTableEx
extern "C" void __stdcall __E__185__()
{
	return;
}

// InternalGetUdpTableWithOwnerModule
extern "C" void __stdcall __E__186__()
{
	return;
}

// InternalGetUdpTableWithOwnerPid
extern "C" void __stdcall __E__187__()
{
	return;
}

// InternalGetUnicastIpAddressEntry
extern "C" void __stdcall __E__188__()
{
	return;
}

// InternalGetUnicastIpAddressTable
extern "C" void __stdcall __E__189__()
{
	return;
}

// InternalIcmpCreateFileEx
extern "C" void __stdcall __E__190__()
{
	return;
}

// InternalSetIfEntry
extern "C" void __stdcall __E__191__()
{
	return;
}

// InternalSetIpForwardEntry
extern "C" void __stdcall __E__192__()
{
	return;
}

// InternalSetIpForwardEntry2
extern "C" void __stdcall __E__193__()
{
	return;
}

// InternalSetIpInterfaceEntry
extern "C" void __stdcall __E__194__()
{
	return;
}

// InternalSetIpNetEntry
extern "C" void __stdcall __E__195__()
{
	return;
}

// InternalSetIpNetEntry2
extern "C" void __stdcall __E__196__()
{
	return;
}

// InternalSetIpStats
extern "C" void __stdcall __E__197__()
{
	return;
}

// InternalSetTcpEntry
extern "C" void __stdcall __E__198__()
{
	return;
}

// InternalSetTeredoPort
extern "C" void __stdcall __E__199__()
{
	return;
}

// InternalSetUnicastIpAddressEntry
extern "C" void __stdcall __E__200__()
{
	return;
}

// IpReleaseAddress
extern "C" void __stdcall __E__201__()
{
	return;
}

// IpRenewAddress
extern "C" void __stdcall __E__202__()
{
	return;
}

// LookupPersistentTcpPortReservation
extern "C" void __stdcall __E__203__()
{
	return;
}

// LookupPersistentUdpPortReservation
extern "C" void __stdcall __E__204__()
{
	return;
}

// NTPTimeToNTFileTime
extern "C" void __stdcall __E__205__()
{
	return;
}

// NTTimeToNTPTime
extern "C" void __stdcall __E__206__()
{
	return;
}

// NhGetGuidFromInterfaceName
extern "C" void __stdcall __E__207__()
{
	return;
}

// NhGetInterfaceDescriptionFromGuid
extern "C" void __stdcall __E__208__()
{
	return;
}

// NhGetInterfaceNameFromDeviceGuid
extern "C" void __stdcall __E__209__()
{
	return;
}

// NhGetInterfaceNameFromGuid
extern "C" void __stdcall __E__210__()
{
	return;
}

// NhpAllocateAndGetInterfaceInfoFromStack
extern "C" void __stdcall __E__211__()
{
	return;
}

// NotifyAddrChange
extern "C" void __stdcall __E__212__()
{
	return;
}

// NotifyCompartmentChange
extern "C" void __stdcall __E__213__()
{
	return;
}

// NotifyIpInterfaceChange
extern "C" void __stdcall __E__214__()
{
	return;
}

// NotifyRouteChange
extern "C" void __stdcall __E__215__()
{
	return;
}

// NotifyRouteChange2
extern "C" void __stdcall __E__216__()
{
	return;
}

// NotifyStableUnicastIpAddressTable
extern "C" void __stdcall __E__217__()
{
	return;
}

// NotifyTeredoPortChange
extern "C" void __stdcall __E__218__()
{
	return;
}

// NotifyUnicastIpAddressChange
extern "C" void __stdcall __E__219__()
{
	return;
}

// OpenCompartment
extern "C" void __stdcall __E__220__()
{
	return;
}

// ParseNetworkString
extern "C" void __stdcall __E__221__()
{
	return;
}

// ResolveIpNetEntry2
extern "C" void __stdcall __E__222__()
{
	return;
}

// ResolveNeighbor
extern "C" void __stdcall __E__223__()
{
	return;
}

// RestoreMediaSense
extern "C" void __stdcall __E__224__()
{
	return;
}

// SendARP
extern "C" void __stdcall __E__225__()
{
	return;
}

// SetAdapterIpAddress
extern "C" void __stdcall __E__226__()
{
	return;
}

// SetCurrentThreadCompartmentId
extern "C" void __stdcall __E__227__()
{
	return;
}

// SetIfEntry
extern "C" void __stdcall __E__228__()
{
	return;
}

// SetIpForwardEntry
extern "C" void __stdcall __E__229__()
{
	return;
}

// SetIpForwardEntry2
extern "C" void __stdcall __E__230__()
{
	return;
}

// SetIpInterfaceEntry
extern "C" void __stdcall __E__231__()
{
	return;
}

// SetIpNetEntry
extern "C" void __stdcall __E__232__()
{
	return;
}

// SetIpNetEntry2
extern "C" void __stdcall __E__233__()
{
	return;
}

// SetIpStatistics
extern "C" void __stdcall __E__234__()
{
	return;
}

// SetIpStatisticsEx
extern "C" void __stdcall __E__235__()
{
	return;
}

// SetIpTTL
extern "C" void __stdcall __E__236__()
{
	return;
}

// SetNetworkInformation
extern "C" void __stdcall __E__237__()
{
	return;
}

// SetPerTcp6ConnectionEStats
extern "C" void __stdcall __E__238__()
{
	return;
}

// SetPerTcp6ConnectionStats
extern "C" void __stdcall __E__239__()
{
	return;
}

// SetPerTcpConnectionEStats
extern "C" void __stdcall __E__240__()
{
	return;
}

// SetPerTcpConnectionStats
extern "C" void __stdcall __E__241__()
{
	return;
}

// SetSessionCompartmentId
extern "C" void __stdcall __E__242__()
{
	return;
}

// SetTcpEntry
extern "C" void __stdcall __E__243__()
{
	return;
}

// SetUnicastIpAddressEntry
extern "C" void __stdcall __E__244__()
{
	return;
}

// UnenableRouter
extern "C" void __stdcall __E__245__()
{
	return;
}

// _PfAddFiltersToInterface@24
extern "C" void __stdcall __E__246__()
{
	return;
}

// _PfAddGlobalFilterToInterface@8
extern "C" void __stdcall __E__247__()
{
	return;
}

// _PfBindInterfaceToIPAddress@12
extern "C" void __stdcall __E__248__()
{
	return;
}

// _PfBindInterfaceToIndex@16
extern "C" void __stdcall __E__249__()
{
	return;
}

// _PfCreateInterface@24
extern "C" void __stdcall __E__250__()
{
	return;
}

// _PfDeleteInterface@4
extern "C" void __stdcall __E__251__()
{
	return;
}

// _PfDeleteLog@0
extern "C" void __stdcall __E__252__()
{
	return;
}

// _PfGetInterfaceStatistics@16
extern "C" void __stdcall __E__253__()
{
	return;
}

// _PfMakeLog@4
extern "C" void __stdcall __E__254__()
{
	return;
}

// _PfRebindFilters@8
extern "C" void __stdcall __E__255__()
{
	return;
}

// _PfRemoveFilterHandles@12
extern "C" void __stdcall __E__256__()
{
	return;
}

// _PfRemoveFiltersFromInterface@20
extern "C" void __stdcall __E__257__()
{
	return;
}

// _PfRemoveGlobalFilterFromInterface@8
extern "C" void __stdcall __E__258__()
{
	return;
}

// _PfSetLogBuffer@28
extern "C" void __stdcall __E__259__()
{
	return;
}

// _PfTestPacket@20
extern "C" void __stdcall __E__260__()
{
	return;
}

// _PfUnBindInterface@4
extern "C" void __stdcall __E__261__()
{
	return;
}

// do_echo_rep
extern "C" void __stdcall __E__262__()
{
	return;
}

// do_echo_req
extern "C" void __stdcall __E__263__()
{
	return;
}

// if_indextoname
extern "C" void __stdcall __E__264__()
{
	return;
}

// if_nametoindex
extern "C" void __stdcall __E__265__()
{
	return;
}

// register_icmp
extern "C" void __stdcall __E__266__()
{
	return;
}

