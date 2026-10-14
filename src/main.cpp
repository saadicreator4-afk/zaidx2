#include "gpu_info.h"
#include "gpu_telemetry.h"
#include "dashboard.h"
static PerfHistory gPerfHistory;
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <sstream>
#include <cwchar>
#include <cstdlib>
#include "app.h"
#include "system_info.h"
#include "optimizer.h"
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"psapi.lib")
using namespace zaidx;
static HWND gInfo{},gLog{},gProc{},gGpu{},gPid{},gSelInfo{};
static void log_line(const std::wstring&s){int n=GetWindowTextLengthW(gLog);SendMessageW(gLog,EM_SETSEL,n,n);SendMessageW(gLog,EM_REPLACESEL,FALSE,(LPARAM)(s+L"\r\n").c_str());}
static unsigned long selected_pid() {
  if (!gProc) return 0;
  LRESULT index = SendMessageW(gProc, LB_GETCURSEL, 0, 0);
  if (index == LB_ERR) return 0;
  wchar_t text[64]{};
  LRESULT copied = SendMessageW(gProc, LB_GETTEXT, (WPARAM)index, (LPARAM)text);
  if (copied == LB_ERR) return 0;
  wchar_t* end = nullptr;
  unsigned long pid = wcstoul(text, &end, 10);
  if (end == text || !end || (*end != L' ' && *end != L'\0') || pid == 0) return 0;
  return pid;
}
static void refresh_info(){auto s=query_system_info();std::wstringstream q;q<<L"CPU threads: "<<s.logical_cpus<<L"\r\nCPU load: "<<(int)s.cpu_load_percent<<L"%\r\nRAM: "<<s.available_ram_mb<<L" MB free / "<<s.total_ram_mb<<L" MB total\r\nGPU: "<<(s.gpu_name.empty()?L"Unknown":s.gpu_name)<<L"\r\nVendor: "<<(s.gpu_vendor.empty()?L"Unknown":s.gpu_vendor)<<L"\r\n"<<get_active_power_plan()<<L"\r\n"<<get_safe_mode_status();SetWindowTextW(gInfo,q.str().c_str());}
static void show_selected();
static void refresh_processes(){
  // Keep the selected PID across refreshes so periodic updates do not change the target.
  const unsigned long keepPid = selected_pid();
  SendMessageW(gProc,LB_RESETCONTENT,0,0);
  std::wstring raw=get_process_list_items(); size_t p=0; int restoreIndex=-1, index=0;
  while(p<raw.size()){
    size_t e=raw.find(L'\n',p); if(e==std::wstring::npos)e=raw.size();
    std::wstring line=raw.substr(p,e-p); if(!line.empty()&&line.back()==L'\r')line.pop_back();
    size_t bar=line.find(L'|');
    if(bar!=std::wstring::npos){
      std::wstring pidText=line.substr(0,bar);
      std::wstring item=pidText+L"  "+line.substr(bar+1);
      LRESULT added = SendMessageW(gProc, LB_ADDSTRING, 0, (LPARAM)item.c_str());
      if (added != LB_ERR && added != LB_ERRSPACE) {
        wchar_t* end=nullptr; unsigned long pid=wcstoul(pidText.c_str(),&end,10);
        if(keepPid && end && end!=pidText.c_str() && *end==L'\0' && pid==keepPid)
          restoreIndex=(int)added;
      } else {
        log_line(L"Process list is full or could not add an item.");
      }
      ++index;
    }
    p=e+1;
  }
  if(restoreIndex>=0) SendMessageW(gProc,LB_SETCURSEL,restoreIndex,0);
  show_selected();
}
static void show_selected(){unsigned long pid=selected_pid();std::wstring name;unsigned long long mb=0;std::wstringstream q;if(pid&&get_process_info(pid,name,mb))q<<name<<L" | PID "<<pid<<L" | Working set "<<mb<<L" MB";else q<<L"No selectable process";SetWindowTextW(gSelInfo,q.str().c_str());}
LRESULT CALLBACK WndProc(HWND h,UINT m,WPARAM w,LPARAM l){
 switch(m){
 case WM_CREATE:{
  CreateWindowW(L"STATIC",L"ZAIDX FPS BOOSTER — Phase 21",WS_CHILD|WS_VISIBLE,20,15,500,30,h,0,0,0);
  gInfo=CreateWindowW(L"STATIC",L"",WS_CHILD|WS_VISIBLE,20,50,540,130,h,0,0,0);
  auto b=[&](LPCWSTR t,int x,int y,int ww,int id){return CreateWindowW(L"BUTTON",t,WS_CHILD|WS_VISIBLE,x,y,ww,30,h,(HMENU)(INT_PTR)id,0,0);};
  b(L"Refresh",20,190,100,101);b(L"Clean TEMP",130,190,110,102);b(L"ZAIDX Priority",250,190,125,103);b(L"High Performance",385,190,150,108);
  gGpu=CreateWindowW(L"EDIT",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_MULTILINE|ES_READONLY|WS_VSCROLL,20,230,540,70,h,0,0,0);
  CreateWindowW(L"STATIC",L"Select target process:",WS_CHILD|WS_VISIBLE,20,310,160,20,h,0,0,0);
  gProc=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|LBS_NOTIFY|WS_VSCROLL,20,335,350,170,h,(HMENU)104,0,0);
  b(L"Normal",385,335,75,106);b(L"High",465,335,75,107);b(L"Affinity Info",385,445,155,109);b(L"Detect Game",385,480,155,110);b(L"Perf Monitor",385,515,155,111);b(L"Gaming Mode",385,550,155,112);b(L"Clear History",385,585,155,113);b(L"Performance Graph",385,620,155,114);b(L"GPU Info",385,655,155,115);
  gSelInfo=CreateWindowW(L"STATIC",L"No selectable process",WS_CHILD|WS_VISIBLE,385,375,165,70,h,0,0,0);
  gLog=CreateWindowW(L"EDIT",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_MULTILINE|ES_READONLY|ES_AUTOVSCROLL|WS_VSCROLL,20,690,540,70,h,0,0,0);
  refresh_info();refresh_processes();SetWindowTextW(gGpu,get_gpu_list_text().c_str());log_line(L"Phase 21 initialized. Source-audit fixes and defensive telemetry checks are enabled.");
  SetTimer(h,1,2000,nullptr);SetTimer(h,2,1000,nullptr);SetTimer(h,3,5000,nullptr);break;}
 case WM_TIMER:
  if(w==1){
    refresh_info();
    auto si = query_system_info();
    double gpu = 0.0;
    bool gpuValid = query_gpu_utilization(gpu);
    PerfSample sample{};
    sample.cpu = si.cpu_load_percent;
    sample.ram_mb = si.total_ram_mb >= si.available_ram_mb ? si.total_ram_mb - si.available_ram_mb : 0;
    sample.ram_percent = si.total_ram_mb ? (100.0 * (double)sample.ram_mb / (double)si.total_ram_mb) : 0.0;
    sample.threads = si.logical_cpus;
    sample.gpu = gpu;
    sample.gpu_valid = gpuValid;
    gPerfHistory.add(sample);
  }
  if(w==3){refresh_processes();}
  if(w==2){
    unsigned long pid=selected_pid();
    if(pid){std::wstring name;unsigned long long mb=0;double cpu=0;unsigned long threads=0;
      if(get_process_info(pid,name,mb)){get_process_cpu_percent(pid,cpu);get_process_thread_count(pid,threads);
        std::wstringstream q;q<<name<<L" | PID "<<pid<<L" | CPU "<<(int)cpu<<L"% | RAM "<<mb<<L" MB | Threads "<<threads;SetWindowTextW(gSelInfo,q.str().c_str());
      } else SetWindowTextW(gSelInfo,L"Process exited or unavailable");
    }
  }
  break;
 case WM_COMMAND:{
  int id=LOWORD(w);
  if(id==104&&HIWORD(w)==LBN_SELCHANGE)show_selected();
  else if(id==101){refresh_info();refresh_processes();SetWindowTextW(gGpu,get_gpu_list_text().c_str());log_line(L"System/GPU/process information refreshed.");}
  else if(id==102){
    const int answer = MessageBoxW(h,
      L"This removes entries inside your current user's TEMP folder. Apps using temporary files may be affected. Continue?",
      L"Confirm TEMP cleanup", MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2);
    if(answer == IDYES){
      std::wstring r;
      clean_user_temp(r);
      log_line(r);
    } else {
      log_line(L"TEMP cleanup cancelled.");
    }
  }
  else if(id==103){std::wstring r;set_current_process_high_priority(r);log_line(r);}
  else if(id==106){
    unsigned long pid=selected_pid();
    if(!pid) log_line(L"Select a process first.");
    else { std::wstring r; set_process_normal_priority(pid,r); log_line(r); }
  }
  else if(id==107){
    unsigned long pid=selected_pid();
    if(!pid) {
      log_line(L"Select a process first.");
    } else if(MessageBoxW(h,
        L"High priority can reduce responsiveness for other apps. Apply it to the selected process?",
        L"Confirm High priority", MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDYES) {
      std::wstring r; set_process_high_priority(pid,r); log_line(r);
    } else {
      log_line(L"High priority change cancelled.");
    }
  }
  else if(id==108){std::wstring r;set_high_performance_plan(r);log_line(r);refresh_info();}
  else if(id==115){auto gs=enumerate_gpus();if(gs.empty())log_line(L"No compatible DXGI GPU found.");else{for(auto& g:gs){std::wstringstream q;q<<g.name<<L" | Dedicated "<<g.dedicated_mb<<L" MB | Shared "<<g.shared_mb<<L" MB";log_line(q.str());}}}
  else if(id==113){gPerfHistory.clear();log_line(L"Performance history cleared.");}
  else if(id==114){show_performance_dashboard((HINSTANCE)GetWindowLongPtrW(h,GWLP_HINSTANCE),h,&gPerfHistory);}
  else if(id==112){
    unsigned long pid=selected_pid();
    if(!pid){
      log_line(L"Select a process first.");
    } else if(MessageBoxW(h,
        L"Gaming Mode sets the selected process to High priority. This is temporary and may affect other apps' responsiveness. Continue?",
        L"Confirm Gaming Mode", MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDYES) {
      std::wstring report;
      if(set_process_high_priority(pid, report))
        log_line(std::wstring(L"Gaming Mode: ") + report + L" Temporary and reversible.");
      else
        log_line(std::wstring(L"Gaming Mode: ") + report);
    } else {
      log_line(L"Gaming Mode cancelled.");
    }
  }
  else if(id==111){unsigned long pid=selected_pid();if(!pid){log_line(L"Select a process first.");}else{std::wstring name;unsigned long long mb=0;double cpu=0;unsigned long threads=0;if(get_process_info(pid,name,mb)){get_process_cpu_percent(pid,cpu);get_process_thread_count(pid,threads);std::wstringstream q;q<<name<<L" | PID "<<pid<<L" | CPU "<<(int)cpu<<L"% | RAM "<<mb<<L" MB | Threads "<<threads;log_line(q.str());}else log_line(L"Selected process is no longer available.");}}
  else if(id==110){unsigned long pid=0;std::wstring name;if(get_foreground_process(pid,name)){std::wstringstream q;q<<L"Foreground: "<<name<<L" | PID "<<pid;if(looks_like_game_process(name))q<<L" | Game-like process detected";else q<<L" | No game signature match";log_line(q.str());}else log_line(L"Could not identify foreground process.");}
  else if(id==109){unsigned long pid=selected_pid(); if(!pid){log_line(L"Select a process first.");} else {DWORD_PTR pm=0,sm=0; HANDLE hp=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid); if(hp && GetProcessAffinityMask(hp,&pm,&sm)){std::wstringstream q;q<<L"PID "<<pid<<L" affinity mask: 0x"<<std::hex<<(unsigned long long)pm<<L" | system mask: 0x"<<(unsigned long long)sm; log_line(q.str());} else log_line(L"Affinity information unavailable."); if(hp)CloseHandle(hp);}}
  break;}
 case WM_DESTROY:KillTimer(h,1);KillTimer(h,2);KillTimer(h,3);PostQuitMessage(0);break;
 default:return DefWindowProcW(h,m,w,l);}
 return 0;}
int WINAPI wWinMain(HINSTANCE hi,HINSTANCE, PWSTR,int show){
 WNDCLASSW wc{};wc.hInstance=hi;wc.lpfnWndProc=WndProc;wc.lpszClassName=L"ZAIDX_MAIN";wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);RegisterClassW(&wc);
 HWND h=CreateWindowW(L"ZAIDX_MAIN",kAppTitle,WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,CW_USEDEFAULT,CW_USEDEFAULT,620,900,nullptr,nullptr,hi,nullptr);
 ShowWindow(h,show);UpdateWindow(h);MSG msg{};while(GetMessageW(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}return(int)msg.wParam;}
