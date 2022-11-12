#include <NImGui/NImGui.hpp>
#include <chrono>
#include <csignal>
#include <ctime>
#include <discord.h>
#include <filesystem>
#include <ini.hpp>
#include <inttypes.h>
#include <iostream>
#include <map>
#include <misc/cpp/imgui_stdlib.h>
#include <thread>
#include <utc.hpp>

#ifdef _MSC_VER
#include <Windows.h>
bool wincons = false;
#endif

#undef LoadIcon

#define VHIST                                                                  \
  "Version History\n\n1.0.0:\n- Inital Version\n\n1.1.0\n- Addet saving your " \
  "settings\n- Addet App icon\n- Some bugfixes\n\n1.2.0\n- Addet Lobby "       \
  "System\n- Some bugfixes\n\n2.0.0\n- Switch to discord game sdk\n- Fake "    \
  "Time\n- Support Small Image\n- Support Image hints\n- Addet Countdown"
bool vhrst = false;

bool rnn = true;

std::string FixFilePaths2BS(std::string path) {
  std::replace(path.begin(), path.end(), '\\', '/');
  return path;
}

std::map<int, std::string> dcerrs = {
    {0, "Ok"},
    {1, "ServiceUnavailable"},
    {2, "InvalidVersion"},
    {3, "LockFailed"},
    {4, "InternalError"},
    {5, "InvalidPayload"},
    {6, "InvalidCommand"},
    {7, "InvalidPermissions"},
    {8, "NotFetched"},
    {9, "NotFound"},
    {10, "Conflict"},
    {11, "InvalidSecret"},
    {12, "InvalidJoinSecret"},
    {13, "NoEligibleActivity"},
    {14, "InvalidInvite"},
    {15, "NotAuthenticated"},
    {16, "InvalidAccessToken"},
    {17, "ApplicationMismatch"},
    {18, "InvalidDataUrl"},
    {19, "InvalidBase64"},
    {20, "NotFiltered"},
    {21, "LobbyFull"},
    {22, "InvalidLobbySecret"},
    {23, "InvalidFilename"},
    {24, "InvalidFileSize"},
    {25, "InvalidEntitlement"},
    {26, "NotInstalled"},
    {27, "NotRunning"},
    {28, "InsufficientBuffer"},
    {29, "PurchaseCanceled"},
    {30, "InvalidGuild"},
    {31, "InvalidEvent"},
    {32, "InvalidChannel"},
    {33, "InvalidOrigin"},
    {34, "RateLimited"},
    {35, "OAuth2Error"},
    {36, "SelectChannelTimeout"},
    {37, "GetGuildTimeout"},
    {38, "SelectVoiceForceRequired"},
    {39, "CaptureShortcutAlreadyListening"},
    {40, "UnauthorizedForAchievement"},
    {41, "InvalidGiftCode"},
    {42, "PurchaseError"},
    {43, "TransactionAborted"},
};

INI::INIStructure st_;

std::string encpath;

bool timeedit = false;

std::string state_ = "";
std::string detail_ = "";
std::string limage_ = "";
std::string simage_ = "";
std::string imagekey_ = "";
std::string simagekey_ = "";
std::string dckey = "";

int lobbysize = 10;
int lobbymembers = 1;

bool disablelobby = false;

// Time
int year_ = 2022;
int month_ = 1;
int day_ = 1;
int hour_ = 0;
int min_ = 0;
int sec_ = 0;
int endyear_ = 2022;
int endmonth_ = 1;
int endday_ = 1;
int endhour_ = 0;
int endmin_ = 0;
int endsec_ = 0;
bool useendtime = false;
NImGui::Timer etm;

std::string username;
std::string dcm;
std::int64_t userid;
int usrerr = 0;

time_t strt;
time_t curt;

bool dcerr = false;
int err = 0;
int acterr = 0;
bool distime = true;
bool faketime = false;

int actll = 0;

const char *actions_l[] = {
    "Playing",
    "Streaming",
    "Listening",
    "Watching",
};
std::string actions_c = actions_l[0];

discord::Core *core{};

int64_t S64(const char *s) {
  int64_t i;
  char c;
  int scanned = sscanf(s, "%" SCNd64 "%c", &i, &c);
  if (scanned == 1)
    return i;
  if (scanned > 1) {
    // TBD about extra data found
    return i;
  }
  // TBD failed to scan;
  return 0;
}

void InitDiscord() {
  auto result = discord::Core::Create(S64(dckey.c_str()),
                                      DiscordCreateFlags_Default, &core);
  if (result != discord::Result::Ok) {
    dcerr = true;
  }
  err = (int)result;
}

void UpdateDCLLA() {
  for (int i = 0; i < IM_ARRAYSIZE(actions_l); i++) {
    if (actions_c == actions_l[i]) {
      actll = i;
    }
  }
}

static void UpdatePresence() {
  discord::Activity activity{};
  activity.SetState(state_.c_str());
  activity.SetDetails(detail_.c_str());

  activity.SetType((discord::ActivityType)actll);
  activity.GetAssets().SetLargeImage(imagekey_.c_str());
  activity.GetAssets().SetSmallImage(simagekey_.c_str());
  activity.GetAssets().SetLargeText(limage_.c_str());
  activity.GetAssets().SetSmallText(simage_.c_str());
  if (distime) {
    if (faketime) {
      if (useendtime) {
        activity.GetTimestamps().SetEnd(
            utc(endyear_, endmonth_, endday_, endhour_, endmin_, endsec_));
      } else {
        // Do Nothing
      }
      if (!useendtime)
        activity.GetTimestamps().SetStart(
            utc(year_, month_, day_, hour_, min_, sec_));
    } else {
      activity.GetTimestamps().SetStart(strt);
    }
  }

  core->ActivityManager().UpdateActivity(
      activity, [](discord::Result result) { acterr = (int)result; });
  discord::User usr;
  usrerr = (int)::core->UserManager().GetCurrentUser(&usr);
  username = usr.GetUsername();
  dcm = usr.GetDiscriminator();
  userid = usr.GetId();

  auto result = ::core->RunCallbacks();
  err = (int)result;
}

void DiscordMain() {
  if (!dcerr)
    UpdatePresence();
}

void Disable() { ::core->~Core(); }

void Bar() {
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Exit", "", false)) {
        rnn = false;
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("About")) {
      if (ImGui::MenuItem("Version History", "", false)) {
        vhrst = !vhrst;
      }
      if (ImGui::MenuItem("Tool Info", "", false)) {
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

void LoadSettings() {
  std::string fname = encpath + "last.ini";
  INI::INIFile f(fname);
  f.read(st_);
  dckey = st_["last"]["key"];
  state_ = st_["last"]["state"];
  imagekey_ = st_["last"]["image"];
  detail_ = st_["last"]["description"];
  disablelobby = (bool)std::atoi(st_["last"]["ldis"].c_str());
  lobbysize = std::atoi(st_["last"]["lsize"].c_str());
  lobbymembers = std::atoi(st_["last"]["lmembers"].c_str());
  year_ = std::atoi(st_["last"]["year"].c_str());
  month_ = std::atoi(st_["last"]["month"].c_str());
  day_ = std::atoi(st_["last"]["day"].c_str());
  hour_ = std::atoi(st_["last"]["hour"].c_str());
  min_ = std::atoi(st_["last"]["min"].c_str());
  sec_ = std::atoi(st_["last"]["sec"].c_str());
  endyear_ = std::atoi(st_["last"]["endyear"].c_str());
  endmonth_ = std::atoi(st_["last"]["endmonth"].c_str());
  endday_ = std::atoi(st_["last"]["endday"].c_str());
  endhour_ = std::atoi(st_["last"]["endhour"].c_str());
  endmin_ = std::atoi(st_["last"]["endmin"].c_str());
  endsec_ = std::atoi(st_["last"]["endsec"].c_str());
  distime = (bool)std::atoi(st_["last"]["distime"].c_str());
  actions_c = st_["last"]["activitytype"];
  simagekey_ = st_["last"]["simagekey"];
  limage_ = st_["last"]["limage"];
  simage_ = st_["last"]["simage"];
  faketime = (bool)std::atoi(st_["last"]["faketime"].c_str());
  useendtime = (bool)std::atoi(st_["last"]["useendtime"].c_str());
}

void SaveSettings() {
  std::string fname = encpath + "last.ini";
  INI::INIFile f(fname);
  st_["last"]["key"] = dckey;
  st_["last"]["state"] = state_;
  st_["last"]["image"] = imagekey_;
  st_["last"]["simagekey"] = simagekey_;
  st_["last"]["description"] = detail_;
  st_["last"]["lmembers"] = std::to_string(lobbymembers);
  st_["last"]["lsize"] = std::to_string(lobbysize);
  st_["last"]["ldis"] = std::to_string(disablelobby);
  st_["last"]["year"] = std::to_string(year_);
  st_["last"]["month"] = std::to_string(month_);
  st_["last"]["day"] = std::to_string(day_);
  st_["last"]["hour"] = std::to_string(hour_);
  st_["last"]["min"] = std::to_string(min_);
  st_["last"]["sec"] = std::to_string(sec_);
  st_["last"]["endyear"] = std::to_string(endyear_);
  st_["last"]["endmonth"] = std::to_string(endmonth_);
  st_["last"]["endday"] = std::to_string(endday_);
  st_["last"]["endhour"] = std::to_string(endhour_);
  st_["last"]["endmin"] = std::to_string(endmin_);
  st_["last"]["endsec"] = std::to_string(endsec_);
  st_["last"]["distime"] = std::to_string(distime);
  st_["last"]["activitytype"] = actions_c;
  st_["last"]["limage"] = limage_;
  st_["last"]["simage"] = simage_;
  st_["last"]["faketime"] = std::to_string(faketime);
  st_["last"]["useendtime"] = std::to_string(useendtime);
  f.generate(st_);
  f.write(st_);
}

void InitEnv() {
  encpath = getenv("appdata");
  encpath += "\\DC-Client\\";
  std::filesystem::create_directories(std::filesystem::path(encpath));
}

std::vector<int> yylst;

int main(int argc, char *argv[]) {
  InitEnv();
  auto dirrr = std::filesystem::weakly_canonical(std::filesystem::path(argv[0]))
                   .parent_path();
  std::cout << dirrr << std::endl;
  std::string thhhh = encpath + "imgui.ini";

#ifdef _MSC_VER
  ::ShowWindow(::GetConsoleWindow(), 0);
#endif // DEBUG

  strt = utcnow();

  if (std::filesystem::exists(encpath + "last.ini")) {
    LoadSettings();
  } else {
    std::string fname = encpath + "last.ini";
    INI::INIFile f(fname);
    st_["last"]["key"] = "Your Key here";
    st_["last"]["state"] = "Online";
    st_["last"]["image"] = "Your image";
    st_["last"]["simagekey"] = "Your image";
    st_["last"]["description"] = "Map: Crash";
    st_["last"]["lmembers"] = std::to_string(1);
    st_["last"]["lsize"] = std::to_string(10);
    st_["last"]["ldis"] = std::to_string(1);
    st_["last"]["year"] = std::to_string(2022);
    st_["last"]["month"] = std::to_string(11);
    st_["last"]["day"] = std::to_string(11);
    st_["last"]["hour"] = std::to_string(11);
    st_["last"]["min"] = std::to_string(11);
    st_["last"]["sec"] = std::to_string(11);
    st_["last"]["endyear"] = std::to_string(2022);
    st_["last"]["endmonth"] = std::to_string(11);
    st_["last"]["endday"] = std::to_string(11);
    st_["last"]["endhour"] = std::to_string(11);
    st_["last"]["endmin"] = std::to_string(11);
    st_["last"]["endsec"] = std::to_string(11);
    st_["last"]["distime"] = std::to_string(1);
    st_["last"]["activitytype"] = actions_c;
    st_["last"]["limage"] = "Your Hint";
    st_["last"]["simage"] = "Your Hint";
    f.generate(st_);
    f.write(st_);
    LoadSettings();
  }

  NImGui::App app("DC-Client (2.0.0)", NImGui::Vec2i(500, 700),
                  NImGui::BORDERLESS | NImGui::NORESIZE | (1 << 4));
  app.LoadIcon(FixFilePaths2BS(dirrr.string()) + "/data/icon.png");
  // ImGui::GetIO().IniFilename = thhhh.c_str();
  ImGui::GetIO().IniFilename = NULL;
  ImGui::Spectrum::LoadFont(22);
  ImGui::Spectrum::StyleColorsSpectrum();
  app.SetWindowPos(
      NImGui::Vec2i((app.GetMonitorSize().x / 2 - app.GetWindowSize().x / 2),
                    (app.GetMonitorSize().y / 2 - app.GetWindowSize().y / 2)));
  InitDiscord();
  yylst = GetYearList();
  while (app.IsRunning() && rnn) {
    UpdateDCLLA();
    if (lobbysize < 0)
      lobbysize = 0;
    if (lobbymembers < 0)
      lobbymembers = 0;
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    ImGui::Begin("DC-Client (2.0.0)", &rnn,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking);
    ImGui::SetWindowSize(ImVec2(app.GetWindowSize().x, app.GetWindowSize().y));
    app.SetWindowPos(
        NImGui::Vec2i(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y));
    Bar();
    ImGui::Text("Empty Fields are Not Displayed in Discord!");
    ImGui::InputText("Key", &dckey);
    ImGui::SameLine();
    if (ImGui::Button("Reload")) {
      Disable();
      InitDiscord();
    }
    ImGui::InputText("State", &state_);
    ImGui::InputText("Detail", &detail_);
    ImGui::InputText("Large Image", &imagekey_);
    ImGui::InputText("Large Image Hint", &limage_);
    ImGui::InputText("Small Image", &simagekey_);
    ImGui::InputText("Small Image Hint", &simage_);

    if (ImGui::BeginCombo("Activity", actions_c.c_str())) {
      for (int n = 0; n < IM_ARRAYSIZE(actions_l); n++) {
        bool is_selected =
            (actions_c ==
             actions_l[n]); // You can store your selection however you want,
                            // outside or inside your objects
        if (ImGui::Selectable(actions_l[n], is_selected))
          actions_c = actions_l[n];
        if (is_selected)
          ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                        // opening the combo (scrolling + for
                                        // keyboard navigation support)
      }
      ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo() returns
                         // true!
    }
    ImGui::Checkbox("Display Time", &distime);
    ImGui::SameLine();
    ImGui::Checkbox("Fake Time", &faketime);
    ImGui::SameLine();
    if (ImGui::Button("Time Editor")) {
      timeedit = !timeedit;
    }

    // ImGui::Checkbox("Disable Lobby", &disablelobby);
    // if (!disablelobby)
    //{
    //	ImGui::InputInt("Lobby Size", &lobbysize);
    //	ImGui::InputInt("Lobby Members", &lobbymembers);
    // }
    ImGui::Text("Discord Dev Portal");
    ImGui::SameLine();
    if (ImGui::Button("Copy Link")) {
      ImGui::SetClipboardText("https://discord.com/developers/applications");
    }
    ImGui::Text("Connected to User:");
    ImGui::Text("User Name -> %s", username.c_str());
    ImGui::Text("User Tag -> %s", dcm.c_str());
    ImGui::Text("User ID -> %d", userid);
    ImGui::Text("Error Code: dc%d(%s)", err, dcerrs.at(err).c_str());
    ImGui::Text("Usr-Error Code: dc%d(%s)", usrerr, dcerrs.at(usrerr).c_str());
    ImGui::Text("Activity-Error: dc%d(%s)", acterr, dcerrs.at(acterr).c_str());
#ifdef _MSC_VER
    if (ImGui::Button("Windows Console")) {
      wincons = !wincons;
      ::ShowWindow(::GetConsoleWindow(), wincons ? 5 : 0);
    }

#endif // DEBUG
    // ImGui::Text("TestKey _> %l", std::atol(dckey.c_str()));

    ImGui::End();
    if (vhrst) {
      ImGui::Begin("Version History", &vhrst,
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoDocking);
      ImGui::Text(VHIST);
      ImGui::End();
    }

    if (timeedit) {
      ImGui::Begin("Time Editor", &timeedit,
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoDocking);
      ImGui::SetWindowSize(ImVec2(240, 600));
      ImGui::Text("Started Playing at:");
      if (ImGui::BeginCombo("Year", std::to_string(year_).c_str())) {
        for (int n = 0; n < yylst.size(); n++) {
          bool is_selected =
              (year_ == yylst[n]); // You can store your selection however you
                                   // want, outside or inside your objects
          if (ImGui::Selectable(std::to_string(yylst[n]).c_str(), is_selected))
            year_ = yylst[n];
          if (is_selected)
            ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                          // opening the combo (scrolling + for
                                          // keyboard navigation support)
        }
        ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo()
                           // returns true!
      }
      if (ImGui::BeginCombo("Month", std::to_string(month_).c_str())) {
        for (int n = 0; n < GetMonthList().size(); n++) {
          bool is_selected =
              (month_ ==
               GetMonthList()[n]); // You can store your selection however you
                                   // want, outside or inside your objects
          if (ImGui::Selectable(std::to_string(GetMonthList()[n]).c_str(),
                                is_selected))
            month_ = GetMonthList()[n];
          if (is_selected)
            ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                          // opening the combo (scrolling + for
                                          // keyboard navigation support)
        }
        ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo()
                           // returns true!
      }
      if (ImGui::BeginCombo("Day", std::to_string(day_).c_str())) {
        for (int n = 0; n < GetDayList().size(); n++) {
          bool is_selected =
              (day_ ==
               GetDayList()[n]); // You can store your selection however you
                                 // want, outside or inside your objects
          if (ImGui::Selectable(std::to_string(GetDayList()[n]).c_str(),
                                is_selected))
            day_ = GetDayList()[n];
          if (is_selected)
            ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                          // opening the combo (scrolling + for
                                          // keyboard navigation support)
        }
        ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo()
                           // returns true!
      }
      if (ImGui::BeginCombo("Hour", std::to_string(hour_).c_str())) {
        for (int n = 0; n < GetHourList().size(); n++) {
          bool is_selected =
              (hour_ ==
               GetHourList()[n]); // You can store your selection however you
                                  // want, outside or inside your objects
          if (ImGui::Selectable(std::to_string(GetHourList()[n]).c_str(),
                                is_selected))
            hour_ = GetHourList()[n];
          if (is_selected)
            ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                          // opening the combo (scrolling + for
                                          // keyboard navigation support)
        }
        ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo()
                           // returns true!
      }
      if (ImGui::BeginCombo("Minute", std::to_string(min_).c_str())) {
        for (int n = 0; n < GetMinSecList().size(); n++) {
          bool is_selected =
              (min_ ==
               GetMinSecList()[n]); // You can store your selection however you
                                    // want, outside or inside your objects
          if (ImGui::Selectable(std::to_string(GetMinSecList()[n]).c_str(),
                                is_selected))
            min_ = GetMinSecList()[n];
          if (is_selected)
            ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                          // opening the combo (scrolling + for
                                          // keyboard navigation support)
        }
        ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo()
                           // returns true!
      }
      if (ImGui::BeginCombo("Second", std::to_string(sec_).c_str())) {
        for (int n = 0; n < GetMinSecList().size(); n++) {
          bool is_selected =
              (sec_ ==
               GetMinSecList()[n]); // You can store your selection however you
                                    // want, outside or inside your objects
          if (ImGui::Selectable(std::to_string(GetMinSecList()[n]).c_str(),
                                is_selected))
            sec_ = GetMinSecList()[n];
          if (is_selected)
            ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                          // opening the combo (scrolling + for
                                          // keyboard navigation support)
        }
        ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo()
                           // returns true!
      }
      if (ImGui::Button("Set to Now")) {
#undef GetCurrentTime
        CTIME_Z gg = GetCurrentTime();
        year_ = gg.ye;
        month_ = gg.mo;
        day_ = gg.da;
        hour_ = gg.h;
        min_ = gg.m;
        sec_ = gg.s;
      }

      ImGui::Text("Count to:");
      if (ImGui::BeginCombo("Year##llg", std::to_string(endyear_).c_str())) {
        for (int n = 0; n < yylst.size(); n++) {
          bool is_selected =
              (endyear_ ==
               yylst[n]); // You can store your selection however you
                          // want, outside or inside your objects
          if (ImGui::Selectable(std::to_string(yylst[n]).c_str(), is_selected))
            endyear_ = yylst[n];
          if (is_selected)
            ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                          // opening the combo (scrolling + for
                                          // keyboard navigation support)
        }
        ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo()
                           // returns true!
      }
      if (ImGui::BeginCombo("Month##llg", std::to_string(endmonth_).c_str())) {
        for (int n = 0; n < GetMonthList().size(); n++) {
          bool is_selected =
              (endmonth_ ==
               GetMonthList()[n]); // You can store your selection however you
                                   // want, outside or inside your objects
          if (ImGui::Selectable(std::to_string(GetMonthList()[n]).c_str(),
                                is_selected))
            endmonth_ = GetMonthList()[n];
          if (is_selected)
            ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                          // opening the combo (scrolling + for
                                          // keyboard navigation support)
        }
        ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo()
                           // returns true!
      }
      if (ImGui::BeginCombo("Day##llg", std::to_string(endday_).c_str())) {
        for (int n = 0; n < GetDayList().size(); n++) {
          bool is_selected =
              (endday_ ==
               GetDayList()[n]); // You can store your selection however you
                                 // want, outside or inside your objects
          if (ImGui::Selectable(std::to_string(GetDayList()[n]).c_str(),
                                is_selected))
            endday_ = GetDayList()[n];
          if (is_selected)
            ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                          // opening the combo (scrolling + for
                                          // keyboard navigation support)
        }
        ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo()
                           // returns true!
      }
      if (ImGui::BeginCombo("Hour##llg", std::to_string(endhour_).c_str())) {
        for (int n = 0; n < GetHourList().size(); n++) {
          bool is_selected =
              (endhour_ ==
               GetHourList()[n]); // You can store your selection however you
                                  // want, outside or inside your objects
          if (ImGui::Selectable(std::to_string(GetHourList()[n]).c_str(),
                                is_selected))
            endhour_ = GetHourList()[n];
          if (is_selected)
            ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                          // opening the combo (scrolling + for
                                          // keyboard navigation support)
        }
        ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo()
                           // returns true!
      }
      if (ImGui::BeginCombo("Minute##llg", std::to_string(endmin_).c_str())) {
        for (int n = 0; n < GetMinSecList().size(); n++) {
          bool is_selected =
              (endmin_ ==
               GetMinSecList()[n]); // You can store your selection however you
                                    // want, outside or inside your objects
          if (ImGui::Selectable(std::to_string(GetMinSecList()[n]).c_str(),
                                is_selected))
            endmin_ = GetMinSecList()[n];
          if (is_selected)
            ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                          // opening the combo (scrolling + for
                                          // keyboard navigation support)
        }
        ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo()
                           // returns true!
      }
      if (ImGui::BeginCombo("Second##llg", std::to_string(endsec_).c_str())) {
        for (int n = 0; n < GetMinSecList().size(); n++) {
          bool is_selected =
              (endsec_ ==
               GetMinSecList()[n]); // You can store your selection however you
                                    // want, outside or inside your objects
          if (ImGui::Selectable(std::to_string(GetMinSecList()[n]).c_str(),
                                is_selected))
            endsec_ = GetMinSecList()[n];
          if (is_selected)
            ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                          // opening the combo (scrolling + for
                                          // keyboard navigation support)
        }
        ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo()
                           // returns true!
      }
      if (ImGui::Button("Set to Now##llg")) {
#undef GetCurrentTime
        CTIME_Z gg = GetCurrentTime();
        endyear_ = gg.ye;
        endmonth_ = gg.mo;
        endday_ = gg.da;
        endhour_ = gg.h;
        endmin_ = gg.m;
        endsec_ = gg.s;
      }
      ImGui::Checkbox("Use as Countdown", &useendtime);
      ImGui::End();
    }

    DiscordMain();
    app.SwapBuffers();
    SaveSettings();
  }

  Disable();

  return 0;
}