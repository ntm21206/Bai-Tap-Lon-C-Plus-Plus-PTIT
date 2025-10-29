// FILE: main_imgui.cpp
#include "imgui.h" // thư viện đồ hoạ
#include "imgui_impl_glfw.h"  //backend GLFW cho imgui
#include "imgui_impl_opengl3.h" // backend openGL3 cho imgui
#include <glad/glad.h>  // openGL loader
#include <GLFW/glfw3.h> //  thư viện tạo cửa sổ và xử lý input
#include<bits/stdc++.h> // thư viện trong c++
#include <fstream>  // đọc ghi file trong c++

using namespace std;

//cấu trúc dữ liệu

//cấu trúc môn học gồm
/*
    tên môn, điểm số, điểm chữ, GPA, số tin chỉ
*/
struct MonHoc 
{
    char tenMon[100]; // tên môn học sử dụng mảng chả thay cho string để có thể dễ lưu file
    float diemSo; // điểm số tính theo thang điểm 10
    char diemChu[5]; // điểm chữ tính theo thang quốc tế gồm a+,a,b+,b,c+,c,d+,d,f
    float diemGPA; // điểm GPA tính theo thang điểm 4, cách tính điểm GPA = tổng điểm môn * số tín chỉ / tổng số tín chỉ trong đó điểm môn thường sẽ được quy đổi dựa trên thang điểm 4 đã quy ước
    int soTinChi; // số tín chỉ thường sẽ tính khoảng từ 3 cho tới 5 tín
    
    MonHoc() 
    {
        strcpy(tenMon, "");
        diemSo = 0.0f;
        strcpy(diemChu, "");
        diemGPA = 0.0f;
        soTinChi = 3;
    }
    
    void chuyenDoiDiem()  // hàm trả về kết quả điểm 
    {
        if (diemSo >= 9.5) { strcpy(diemChu, "A+"); diemGPA = 4.0f; }
        else if (diemSo >= 8.5) { strcpy(diemChu, "A"); diemGPA = 3.7f; }
        else if (diemSo >= 8.0) { strcpy(diemChu, "B+"); diemGPA = 3.5f; }
        else if (diemSo >= 7.0) { strcpy(diemChu, "B"); diemGPA = 3.0f; }
        else if (diemSo >= 6.5) { strcpy(diemChu, "C+"); diemGPA = 2.5f; }
        else if (diemSo >= 5.5) { strcpy(diemChu, "C"); diemGPA = 2.0f; }
        else if (diemSo >= 5.0) { strcpy(diemChu, "D+"); diemGPA = 1.5f; }
        else if (diemSo >= 4.0) { strcpy(diemChu, "D"); diemGPA = 1.0f; }
        else { strcpy(diemChu, "F"); diemGPA = 0.0f; }
    }
};

// cấu trúc học sinh bao gồm
/*
    tên, mã sv, môn học, mail, xếp loại
*/

struct HocSinh 
{
    char hoTen[100]; // biến để lưu tên học sinh kiểu chuỗi kí tự
    char maSV[20]; //mã sinh viên, kiểu chuỗi kí tự 
    char email[100]; // email học sinh kiểu chuỗi kí tự
    vector<MonHoc> danhSachMon; // danh sách các môn học mà sv đã học, sử dụng mảng vector thay vì mảng thông thường vì vector là mảng động k phải mảng tĩnh
    float gpa; // điểm trung bình tích luỹ
    char xepLoai[20]; // xếp loại học lực
    
    HocSinh() // ban đầu khởi tạo các biến họ tên mã sv email xếp loại đều rỗng và gpa ban đầu cho = 0
    {
        strcpy(hoTen, "");
        strcpy(maSV, "");
        strcpy(email, "");
        gpa = 0.0f;
        strcpy(xepLoai, "");
    }
    
    void tinhGPA() // hàm để tính GPA
    {
        if (danhSachMon.empty()) 
        {
            gpa = 0.0f;
            return;
        }
        
        float tongDiem = 0.0f;
        int tongTinChi = 0;
        
        for (const auto& mon : danhSachMon) 
        {
            tongDiem += mon.diemGPA * mon.soTinChi;
            tongTinChi += mon.soTinChi;
        }
        
        gpa = (tongTinChi > 0) ? tongDiem / tongTinChi : 0.0f;
        
        if (gpa >= 3.6) strcpy(xepLoai, "Xuat sac");
        else if (gpa >= 3.2) strcpy(xepLoai, "Gioi");
        else if (gpa >= 2.5) strcpy(xepLoai, "Kha");
        else if (gpa >= 2.0) strcpy(xepLoai, "Trung binh");
        else strcpy(xepLoai, "Yeu");
    }
    
    ImVec4 getMauXepLoai() const  // hàm trả về màu sắc đại diện cho các xếp loại học lực
    {
        if (strcmp(xepLoai, "Xuat sac") == 0) return ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
        else if (strcmp(xepLoai, "Gioi") == 0) return ImVec4(0.0f, 0.8f, 1.0f, 1.0f);
        else if (strcmp(xepLoai, "Kha") == 0) return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        else if (strcmp(xepLoai, "Trung binh") == 0) return ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
        else return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
};

// Lớp quản lý điểm dùng imgui để nền đồ hoạ
class QuanLyDiemGUI 
{
private:
    vector<HocSinh> danhSachHS;
    
    // Trạng thái UI
    int selectedStudent = -1;
    bool showAddDialog = false;
    bool showDetailDialog = false;
    bool showStatsDialog = false;
    
    // Form thêm sinh viên
    char inputHoTen[100] = "";
    char inputMaSV[20] = "";
    char inputEmail[100] = "";
    char inputTenMon[100] = "";
    float inputDiem = 0.0f;
    int inputTinChi = 3;
    vector<MonHoc> tempMonHoc;
    
    // Thống kê
    struct ThongKe 
    {
        int tongSV = 0;
        float gpaAvg = 0.0f;
        float gpaMax = 0.0f;
        float gpaMin = 4.0f;
        int xuatSac = 0, gioi = 0, kha = 0, trungBinh = 0, yeu = 0;
    } stats;
    
public:
    void capNhatThongKe() {
        stats = ThongKe();
        stats.tongSV = danhSachHS.size();
        
        if (danhSachHS.empty()) return;
        
        float tongGPA = 0.0f;
        for (const auto& hs : danhSachHS) {
            tongGPA += hs.gpa;
            stats.gpaMax = max(stats.gpaMax, hs.gpa);
            stats.gpaMin = min(stats.gpaMin, hs.gpa);
            
            if (strcmp(hs.xepLoai, "Xuat sac") == 0) stats.xuatSac++;
            else if (strcmp(hs.xepLoai, "Gioi") == 0) stats.gioi++;
            else if (strcmp(hs.xepLoai, "Kha") == 0) stats.kha++;
            else if (strcmp(hs.xepLoai, "Trung binh") == 0) stats.trungBinh++;
            else stats.yeu++;
        }
        
        stats.gpaAvg = tongGPA / stats.tongSV;
    }
    
    void renderMainWindow() {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        
        ImGui::Begin("QUAN LY DIEM - THANG 4.0", nullptr, 
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
        
        // Menu Bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Them sinh vien", "Ctrl+N")) showAddDialog = true;
                if (ImGui::MenuItem("Luu du lieu", "Ctrl+S")) luuFile();
                if (ImGui::MenuItem("Doc du lieu", "Ctrl+O")) docFile();
                ImGui::Separator();
                if (ImGui::MenuItem("Xuat CSV")) xuatCSV();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Xem")) {
                if (ImGui::MenuItem("Thong ke")) showStatsDialog = true;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        
        // Toolbar
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        if (ImGui::Button("+ Them SV", ImVec2(120, 30))) showAddDialog = true;
        ImGui::PopStyleColor();
        
        ImGui::SameLine();
        if (ImGui::Button("Luu", ImVec2(80, 30))) luuFile();
        ImGui::SameLine();
        if (ImGui::Button("Doc", ImVec2(80, 30))) docFile();
        ImGui::SameLine();
        if (ImGui::Button("Thong ke", ImVec2(100, 30))) showStatsDialog = true;
        ImGui::SameLine();
        
        // Sắp xếp
        ImGui::Text(" | Sap xep:");
        ImGui::SameLine();
        if (ImGui::Button("GPA", ImVec2(60, 30))) {
            sort(danhSachHS.begin(), danhSachHS.end(), 
                 [](const HocSinh& a, const HocSinh& b) { return a.gpa > b.gpa; });
        }
        ImGui::SameLine();
        if (ImGui::Button("Ten", ImVec2(60, 30))) {
            sort(danhSachHS.begin(), danhSachHS.end(), 
                 [](const HocSinh& a, const HocSinh& b) { 
                     return strcmp(a.hoTen, b.hoTen) < 0; 
                 });
        }
        
        ImGui::Separator();
        
        // Hiển thị số lượng
        ImGui::Text("Tong sinh vien: %d", (int)danhSachHS.size());
        ImGui::Separator();
        
        // Bảng danh sách
        ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | 
                                ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable;
        
        if (ImGui::BeginTable("DanhSachSV", 5, flags, ImVec2(0, -30))) {
            ImGui::TableSetupColumn("STT", ImGuiTableColumnFlags_WidthFixed, 50);
            ImGui::TableSetupColumn("Ma SV", ImGuiTableColumnFlags_WidthFixed, 100);
            ImGui::TableSetupColumn("Ho ten", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("GPA", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableSetupColumn("Xep loai", ImGuiTableColumnFlags_WidthFixed, 120);
            ImGui::TableHeadersRow();
            
            for (int i = 0; i < danhSachHS.size(); i++) {
                ImGui::TableNextRow();
                
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", i + 1);
                
                ImGui::TableSetColumnIndex(1);
                if (ImGui::Selectable(danhSachHS[i].maSV, selectedStudent == i, 
                                     ImGuiSelectableFlags_SpanAllColumns)) {
                    selectedStudent = i;
                    showDetailDialog = true;
                }
                
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", danhSachHS[i].hoTen);
                
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.2f", danhSachHS[i].gpa);
                
                ImGui::TableSetColumnIndex(4);
                ImGui::PushStyleColor(ImGuiCol_Text, danhSachHS[i].getMauXepLoai());
                ImGui::Text("%s", danhSachHS[i].xepLoai);
                ImGui::PopStyleColor();
            }
            
            ImGui::EndTable();
        }
        
        ImGui::End();
    }
    
    void renderAddDialog() {
        if (!showAddDialog) return;
        
        ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), 
                                ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        
        ImGui::Begin("Them Sinh Vien", &showAddDialog);
        
        ImGui::Text("THONG TIN CO BAN:");
        ImGui::Separator();
        ImGui::InputText("Ho ten", inputHoTen, 100);
        ImGui::InputText("Ma SV (VD: SV123456)", inputMaSV, 20);
        ImGui::InputText("Email", inputEmail, 100);
        
        ImGui::Spacing();
        ImGui::Text("THEM MON HOC:");
        ImGui::Separator();
        
        ImGui::InputText("Ten mon", inputTenMon, 100);
        ImGui::SliderFloat("Diem (0-10)", &inputDiem, 0.0f, 10.0f, "%.1f");
        ImGui::SliderInt("Tin chi", &inputTinChi, 1, 5);
        
        if (ImGui::Button("Them mon", ImVec2(150, 30))) {
            if (strlen(inputTenMon) > 0) {
                MonHoc mon;
                strcpy(mon.tenMon, inputTenMon);
                mon.diemSo = inputDiem;
                mon.soTinChi = inputTinChi;
                mon.chuyenDoiDiem();
                tempMonHoc.push_back(mon);
                
                strcpy(inputTenMon, "");
                inputDiem = 0.0f;
                inputTinChi = 3;
            }
        }
        
        // Hiển thị danh sách môn đã thêm
        if (!tempMonHoc.empty()) {
            ImGui::Spacing();
            ImGui::Text("Danh sach mon da them (%d):", (int)tempMonHoc.size());
            ImGui::Separator();
            
            if (ImGui::BeginTable("MonHoc", 5, ImGuiTableFlags_Borders)) {
                ImGui::TableSetupColumn("Ten mon");
                ImGui::TableSetupColumn("Diem");
                ImGui::TableSetupColumn("Chu");
                ImGui::TableSetupColumn("GPA");
                ImGui::TableSetupColumn("TC");
                ImGui::TableHeadersRow();
                
                for (const auto& mon : tempMonHoc) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", mon.tenMon);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%.1f", mon.diemSo);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", mon.diemChu);
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%.1f", mon.diemGPA);
                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("%d", mon.soTinChi);
                }
                
                ImGui::EndTable();
            }
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        if (ImGui::Button("LUU SINH VIEN", ImVec2(150, 35))) {
            if (strlen(inputHoTen) > 0 && strlen(inputMaSV) > 0 && !tempMonHoc.empty()) {
                HocSinh hs;
                strcpy(hs.hoTen, inputHoTen);
                strcpy(hs.maSV, inputMaSV);
                strcpy(hs.email, inputEmail);
                hs.danhSachMon = tempMonHoc;
                hs.tinhGPA();
                
                danhSachHS.push_back(hs);
                capNhatThongKe();
                
                // Reset form
                strcpy(inputHoTen, "");
                strcpy(inputMaSV, "");
                strcpy(inputEmail, "");
                tempMonHoc.clear();
                
                showAddDialog = false;
            }
        }
        ImGui::PopStyleColor();
        
        ImGui::SameLine();
        if (ImGui::Button("HUY", ImVec2(150, 35))) {
            showAddDialog = false;
            tempMonHoc.clear();
        }
        
        ImGui::End();
    }
    
    void renderDetailDialog() {
        if (!showDetailDialog || selectedStudent < 0 || selectedStudent >= danhSachHS.size()) 
            return;
        
        ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), 
                                ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        
        ImGui::Begin("Chi Tiet Sinh Vien", &showDetailDialog);
        
        HocSinh& hs = danhSachHS[selectedStudent];
        
        ImGui::Text("Ma SV: %s", hs.maSV);
        ImGui::Text("Ho ten: %s", hs.hoTen);
        ImGui::Text("Email: %s", hs.email);
        
        ImGui::Separator();
        
        ImGui::PushStyleColor(ImGuiCol_Text, hs.getMauXepLoai());
        ImGui::Text("GPA: %.2f / 4.0", hs.gpa);
        ImGui::Text("Xep loai: %s", hs.xepLoai);
        ImGui::PopStyleColor();
        
        ImGui::Separator();
        ImGui::Text("BANG DIEM CHI TIET:");
        
        if (ImGui::BeginTable("BangDiem", 5, ImGuiTableFlags_Borders)) {
            ImGui::TableSetupColumn("Mon hoc");
            ImGui::TableSetupColumn("Diem");
            ImGui::TableSetupColumn("Chu");
            ImGui::TableSetupColumn("GPA");
            ImGui::TableSetupColumn("TC");
            ImGui::TableHeadersRow();
            
            for (const auto& mon : hs.danhSachMon) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", mon.tenMon);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.1f", mon.diemSo);
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", mon.diemChu);
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.1f", mon.diemGPA);
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%d", mon.soTinChi);
            }
            
            ImGui::EndTable();
        }
        
        ImGui::End();
    }
    
    void renderStatsDialog() {
        if (!showStatsDialog) return;
        
        ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), 
                                ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        
        ImGui::Begin("Thong Ke", &showStatsDialog);
        
        capNhatThongKe();
        
        ImGui::Text("TONG QUAN:");
        ImGui::Separator();
        ImGui::Text("Tong sinh vien: %d", stats.tongSV);
        ImGui::Text("GPA trung binh: %.2f / 4.0", stats.gpaAvg);
        ImGui::Text("GPA cao nhat: %.2f", stats.gpaMax);
        ImGui::Text("GPA thap nhat: %.2f", stats.gpaMin);
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("PHAN BO XEP LOAI:");
        ImGui::Spacing();
        
        // Biểu đồ cột
        float maxVal = max({(float)stats.xuatSac, (float)stats.gioi, (float)stats.kha, 
                           (float)stats.trungBinh, (float)stats.yeu});
        
        if (maxVal > 0 && stats.tongSV > 0) {
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            ImGui::ProgressBar(stats.xuatSac / maxVal, ImVec2(-1, 30));
            ImGui::SameLine(0, 10);
            ImGui::Text("Xuat sac: %d (%.1f%%)", stats.xuatSac, stats.xuatSac * 100.0f / stats.tongSV);
            ImGui::PopStyleColor();
            
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
            ImGui::ProgressBar(stats.gioi / maxVal, ImVec2(-1, 30));
            ImGui::SameLine(0, 10);
            ImGui::Text("Gioi: %d (%.1f%%)", stats.gioi, stats.gioi * 100.0f / stats.tongSV);
            ImGui::PopStyleColor();
            
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            ImGui::ProgressBar(stats.kha / maxVal, ImVec2(-1, 30));
            ImGui::SameLine(0, 10);
            ImGui::Text("Kha: %d (%.1f%%)", stats.kha, stats.kha * 100.0f / stats.tongSV);
            ImGui::PopStyleColor();
            
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
            ImGui::ProgressBar(stats.trungBinh / maxVal, ImVec2(-1, 30));
            ImGui::SameLine(0, 10);
            ImGui::Text("Trung binh: %d (%.1f%%)", stats.trungBinh, stats.trungBinh * 100.0f / stats.tongSV);
            ImGui::PopStyleColor();
            
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::ProgressBar(stats.yeu / maxVal, ImVec2(-1, 30));
            ImGui::SameLine(0, 10);
            ImGui::Text("Yeu: %d (%.1f%%)", stats.yeu, stats.yeu * 100.0f / stats.tongSV);
            ImGui::PopStyleColor();
        }
        
        ImGui::End();
    }
    
    void luuFile() {
        ofstream file("danhsach_gui.dat");
        if (!file.is_open()) return;
        
        file << danhSachHS.size() << "\n";
        for (const auto& hs : danhSachHS) {
            file << hs.maSV << "\n" << hs.hoTen << "\n" << hs.email << "\n";
            file << hs.danhSachMon.size() << "\n";
            for (const auto& mon : hs.danhSachMon) {
                file << mon.tenMon << "\n" << mon.diemSo << "\n" << mon.soTinChi << "\n";
            }
        }
        file.close();
    }
    
    void docFile() {
        ifstream file("danhsach_gui.dat");
        if (!file.is_open()) return;
        
        danhSachHS.clear();
        int soHS;
        file >> soHS;
        file.ignore();
        
        for (int i = 0; i < soHS; i++) {
            HocSinh hs;
            file.getline(hs.maSV, 20);
            file.getline(hs.hoTen, 100);
            file.getline(hs.email, 100);
            
            int soMon;
            file >> soMon;
            file.ignore();
            
            for (int j = 0; j < soMon; j++) {
                MonHoc mon;
                file.getline(mon.tenMon, 100);
                file >> mon.diemSo >> mon.soTinChi;
                file.ignore();
                mon.chuyenDoiDiem();
                hs.danhSachMon.push_back(mon);
            }
            
            hs.tinhGPA();
            danhSachHS.push_back(hs);
        }
        
        file.close();
        capNhatThongKe();
    }
    
    void xuatCSV() {
        ofstream file("danhsach_export.csv");
        if (!file.is_open()) return;
        
        file << "Ma SV,Ho ten,Email,GPA,Xep loai\n";
        for (const auto& hs : danhSachHS) {
            file << hs.maSV << "," << hs.hoTen << "," << hs.email << "," 
                 << hs.gpa << "," << hs.xepLoai << "\n";
        }
        
        file.close();
    }
    
    void render() {
        renderMainWindow();
        renderAddDialog();
        renderDetailDialog();
        renderStatsDialog();
    }
};

// ==================== MAIN ====================
int main() {
    // Khởi tạo GLFW
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Quan Ly Diem GPA - Professional Edition", NULL, NULL);
    if (window == NULL) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD" << endl;
        return -1;
    }
    
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    
    // Custom style
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    QuanLyDiemGUI app;
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        app.render();
        
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
    }
    
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}