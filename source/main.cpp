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
  "Version History\n\n1.0.0:\nInital Version\n\n1.1.0\n- Addet saving your "   \
  "settings\n- Addet App icon\n- Some bugfixes\n\n1.2.0\n- Addet Lobby "       \
  "System\n- Some bugfixes\n\n2.0.0\nSwitch to discord game sdk"
bool vhrst = false;

bool rnn = true;

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

std::string state_ = "Los Santos";
std::string detail_ = "Online";
std::string limage_ = "VI";
std::string simage_ = "VI";
std::string imagekey_ = "z22";
std::string simagekey_ = "z22";
std::string dckey = "1039167502858588180";

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

    // std::localtime(&curt);
    // std::cout << utc(2022, 11, 12, 8, 50, 23) << std::endl;
    activity.GetTimestamps().SetStart(strt);
    // activity.GetTimestamps().SetEnd(utc(2022, 11, 12, 9, 30, 23));
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
  distime = (bool)std::atoi(st_["last"]["distime"].c_str());
  actions_c = st_["last"]["activitytype"];
  simagekey_ = st_["last"]["simagekey"];
  limage_ = st_["last"]["limage"];
  simage_ = st_["last"]["simage"];
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
  st_["last"]["distime"] = std::to_string(distime);
  st_["last"]["activitytype"] = actions_c;
  st_["last"]["limage"] = limage_;
  st_["last"]["simage"] = simage_;
  f.generate(st_);
  f.write(st_);
}

void InitEnv() {
  encpath = getenv("appdata");
  encpath += "\\DC-Client\\";
  std::filesystem::create_directories(std::filesystem::path(encpath));
}

int main() {
  InitEnv();
  std::string thhhh = encpath + "imgui.ini";

#ifdef _MSC_VER
  ::ShowWindow(::GetConsoleWindow(), 0);
#endif // DEBUG

  strt = utcnow();

  if (std::filesystem::exists(encpath + "last.ini")) {
    LoadSettings();
  }

  NImGui::App app("DC-Client (2.0.0)", NImGui::Vec2i(400, 700),
                  NImGui::BORDERLESS | NImGui::NORESIZE | (1 << 4));
  ImGui::GetIO().IniFilename = thhhh.c_str();
  ImGui::Spectrum::LoadFont(22);
  ImGui::Spectrum::StyleColorsSpectrum();
  app.SetWindowPos(
      NImGui::Vec2i((app.GetMonitorSize().x / 2 - app.GetWindowSize().x / 2),
                    (app.GetMonitorSize().y / 2 - app.GetWindowSize().y / 2)));
  InitDiscord();
  while (app.IsRunning() && rnn) {
    if (lobbysize < 0)
      lobbysize = 0;
    if (lobbymembers < 0)
      lobbymembers = 0;
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    ImGui::Begin("DC-Client (2.0.0)", &rnn,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_MenuBar);
    ImGui::SetWindowSize(ImVec2(app.GetWindowSize().x, app.GetWindowSize().y));
    app.SetWindowPos(
        NImGui::Vec2i(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y));
    Bar();
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
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
      ImGui::Text(VHIST);
      ImGui::End();
    }
    DiscordMain();
    app.SwapBuffers();
    SaveSettings();
  }

  Disable();

  return 0;
}