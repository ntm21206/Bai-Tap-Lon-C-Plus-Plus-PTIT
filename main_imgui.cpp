#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#include <fstream>

using namespace std;

struct MonHoc {
    char tenMon[100];
    float diemSo;
    char diemChu[5];
    float diemGPA;
    int soTinChi;
    
    MonHoc() {
        strcpy(tenMon, "");
        diemSo = 0.0f;
        strcpy(diemChu, "");
        diemGPA = 0.0f;
        soTinChi = 3;
    }
    
    void chuyenDoiDiem() {
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

struct HocSinh {
    char hoTen[100];
    char maSV[20];
    char email[100];
    vector<MonHoc> danhSachMon;
    float gpa;
    char xepLoai[20];
    
    HocSinh() {
        strcpy(hoTen, "");
        strcpy(maSV, "");
        strcpy(email, "");
        gpa = 0.0f;
        strcpy(xepLoai, "");
    }
    
    void tinhGPA() {
        if (danhSachMon.empty()) {
            gpa = 0.0f;
            return;
        }
        
        float tongDiem = 0.0f;
        int tongTinChi = 0;
        
        for (const auto& mon : danhSachMon) {
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
    
    ImVec4 getMauXepLoai() const {
        if (strcmp(xepLoai, "Xuat sac") == 0) return ImVec4(0.2f, 0.9f, 0.4f, 1.0f);
        else if (strcmp(xepLoai, "Gioi") == 0) return ImVec4(0.3f, 0.7f, 1.0f, 1.0f);
        else if (strcmp(xepLoai, "Kha") == 0) return ImVec4(1.0f, 0.8f, 0.2f, 1.0f);
        else if (strcmp(xepLoai, "Trung binh") == 0) return ImVec4(1.0f, 0.5f, 0.2f, 1.0f);
        else return ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
    }
};

class QuanLyDiemGUI {
private:
    vector<HocSinh> danhSachHS;
    int selectedStudent = -1;
    bool showAddDialog = false;
    bool showDetailDialog = false;
    bool showStatsDialog = false;
    bool showDeleteConfirm = false;
    int deleteStudentIndex = -1;
    char searchBuffer[100] = "";
    
    char inputHoTen[100] = "";
    char inputMaSV[20] = "";
    char inputEmail[100] = "";
    char inputTenMon[100] = "";
    float inputDiem = 0.0f;
    int inputTinChi = 3;
    vector<MonHoc> tempMonHoc;
    
    struct ThongKe {
        int tongSV = 0;
        float gpaAvg = 0.0f;
        float gpaMax = 0.0f;
        float gpaMin = 4.0f;
        int xuatSac = 0, gioi = 0, kha = 0, trungBinh = 0, yeu = 0;
    } stats;
    
    // Animation variables
    float animTime = 0.0f;
    
    void applyModernStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        
        // Rounded corners
        style.WindowRounding = 12.0f;
        style.ChildRounding = 8.0f;
        style.FrameRounding = 6.0f;
        style.PopupRounding = 8.0f;
        style.ScrollbarRounding = 12.0f;
        style.GrabRounding = 6.0f;
        style.TabRounding = 8.0f;
        
        // Spacing
        style.WindowPadding = ImVec2(15, 15);
        style.FramePadding = ImVec2(10, 6);
        style.ItemSpacing = ImVec2(12, 8);
        style.ItemInnerSpacing = ImVec2(8, 6);
        style.IndentSpacing = 25.0f;
        
        // Borders
        style.WindowBorderSize = 0.0f;
        style.ChildBorderSize = 1.0f;
        style.FrameBorderSize = 0.0f;
        
        // Colors - Modern Dark Theme with accent colors
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.11f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.14f, 0.98f);
        colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.28f, 0.60f);
        
        colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.20f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.30f, 1.00f);
        
        colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.09f, 0.11f, 1.00f);
        
        colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        
        colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.50f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        
        colors[ImGuiCol_Tab] = ImVec4(0.16f, 0.16f, 0.20f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.45f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.55f, 1.00f);
        
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.16f, 0.16f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.25f, 0.25f, 0.28f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.20f, 0.20f, 0.23f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);
    }
    
    void drawGradientRect(ImVec2 pos, ImVec2 size, ImU32 col1, ImU32 col2, bool horizontal = true) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        if (horizontal) {
            draw_list->AddRectFilledMultiColor(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                col1, col2, col2, col1);
        } else {
            draw_list->AddRectFilledMultiColor(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                col1, col1, col2, col2);
        }
    }
    
public:
    QuanLyDiemGUI() {
        applyModernStyle();
    }
    
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
        animTime += ImGui::GetIO().DeltaTime;
        
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        
        ImGui::Begin("##MainWindow", nullptr, 
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        
        // Header with gradient
        ImVec2 headerPos = ImGui::GetCursorScreenPos();
        drawGradientRect(headerPos, ImVec2(ImGui::GetContentRegionAvail().x, 100),
                        IM_COL32(41, 98, 255, 255), IM_COL32(20, 50, 150, 255));
        
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
        ImGui::SetCursorPosX(30);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::Text("HỆ THỐNG QUẢN LÝ ĐIỂM GPA");
        ImGui::SetCursorPosX(30);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
        ImGui::Text("Đào tạo theo hệ tín chỉ");
        ImGui::PopStyleColor(2);
        ImGui::PopFont();
        
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        
        // Main content area
        ImGui::BeginChild("ContentArea", ImVec2(0, 0), false);
        
        // Stats cards
        ImGui::Columns(4, "StatsCards", false);
        
        // Card 1: Total Students
        ImGui::BeginChild("Card1", ImVec2(0, 100), true, ImGuiWindowFlags_NoScrollbar);
        ImGui::SetCursorPosY(15);
        ImGui::SetCursorPosX(20);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.8f, 1.0f, 1.0f));
        ImGui::Text("SINH VIÊN");
        ImGui::PopStyleColor();
        ImGui::SetCursorPosX(20);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::Text("%d", (int)danhSachHS.size());
        ImGui::PopStyleColor();
        ImGui::EndChild();
        
        ImGui::NextColumn();
        
        // Card 2: Average GPA
        ImGui::BeginChild("Card2", ImVec2(0, 100), true, ImGuiWindowFlags_NoScrollbar);
        ImGui::SetCursorPosY(15);
        ImGui::SetCursorPosX(20);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 1.0f, 0.6f, 1.0f));
        ImGui::Text("GPA TB");
        ImGui::PopStyleColor();
        ImGui::SetCursorPosX(20);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        capNhatThongKe();
        ImGui::Text("%.2f", stats.gpaAvg);
        ImGui::PopStyleColor();
        ImGui::EndChild();
        
        ImGui::NextColumn();
        
        // Card 3: Max GPA
        ImGui::BeginChild("Card3", ImVec2(0, 100), true, ImGuiWindowFlags_NoScrollbar);
        ImGui::SetCursorPosY(15);
        ImGui::SetCursorPosX(20);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
        ImGui::Text("CAO NHẤT");
        ImGui::PopStyleColor();
        ImGui::SetCursorPosX(20);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::Text("%.2f", stats.gpaMax);
        ImGui::PopStyleColor();
        ImGui::EndChild();
        
        ImGui::NextColumn();
        
        // Card 4: Excellent Students
        ImGui::BeginChild("Card4", ImVec2(0, 100), true, ImGuiWindowFlags_NoScrollbar);
        ImGui::SetCursorPosY(15);
        ImGui::SetCursorPosX(20);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.8f, 1.0f));
        ImGui::Text("XUẤT SẮC");
        ImGui::PopStyleColor();
        ImGui::SetCursorPosX(20);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::Text("%d", stats.xuatSac);
        ImGui::PopStyleColor();
        ImGui::EndChild();
        
        ImGui::Columns(1);
        ImGui::Spacing();
        
        // Action buttons
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.9f, 0.4f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.7f, 0.2f, 1.0f));
        if (ImGui::Button("Thêm Sinh Viên", ImVec2(180, 40))) 
            showAddDialog = true;
        ImGui::PopStyleColor(3);
        
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 0.9f, 1.0f));
        if (ImGui::Button("Thống Kê", ImVec2(140, 40))) 
            showStatsDialog = true;
        ImGui::PopStyleColor(3);
        
        ImGui::SameLine();
        if (ImGui::Button("Lưu", ImVec2(100, 40))) 
            luuFile();
        ImGui::SameLine();
        if (ImGui::Button("Đọc", ImVec2(100, 40))) 
            docFile();
        ImGui::SameLine();
        if (ImGui::Button("CSV", ImVec2(100, 40))) 
            xuatCSV();
        
        ImGui::PopStyleVar();
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Search and filter
        ImGui::SetNextItemWidth(300);
        ImGui::InputTextWithHint("##search", "Tìm kiếm sinh viên", searchBuffer, 100);
        ImGui::SameLine();
        ImGui::Text("Sắp xếp:");
        ImGui::SameLine();
        if (ImGui::Button("GPA ↓")) {
            sort(danhSachHS.begin(), danhSachHS.end(), 
                 [](const HocSinh& a, const HocSinh& b) { return a.gpa > b.gpa; });
        }
        ImGui::SameLine();
        if (ImGui::Button("Tên A-Z")) {
            sort(danhSachHS.begin(), danhSachHS.end(), 
                 [](const HocSinh& a, const HocSinh& b) { 
                     return strcmp(a.hoTen, b.hoTen) < 0; 
                 });
        }
        
        ImGui::Spacing();
        
        // Student table
        ImGui::BeginChild("TableArea", ImVec2(0, 0), true);
        
        ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | 
                                ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable |
                                ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingStretchProp;
        
        if (ImGui::BeginTable("StudentTable", 7, flags)) {
            ImGui::TableSetupColumn("STT", ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableSetupColumn("Mã SV", ImGuiTableColumnFlags_WidthFixed, 120);
            ImGui::TableSetupColumn("Họ và Tên", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Email", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("GPA", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableSetupColumn("Xếp Loại", ImGuiTableColumnFlags_WidthFixed, 130);
            ImGui::TableSetupColumn("Thao tác", ImGuiTableColumnFlags_WidthFixed, 100);
            ImGui::TableHeadersRow();
            
            for (int i = 0; i < danhSachHS.size(); i++) {
                // Search filter
                if (strlen(searchBuffer) > 0) {
                    if (strstr(danhSachHS[i].hoTen, searchBuffer) == nullptr &&
                        strstr(danhSachHS[i].maSV, searchBuffer) == nullptr) {
                        continue;
                    }
                }
                
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", i + 1);
                
                ImGui::TableSetColumnIndex(1);
                if (ImGui::Selectable(danhSachHS[i].maSV, selectedStudent == i, 
                                     ImGuiSelectableFlags_SpanAllColumns)) {
                    selectedStudent = i;
                    showDetailDialog = true;
                }
                
                if (ImGui::IsItemHovered()) {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                }
                
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", danhSachHS[i].hoTen);
                
                ImGui::TableSetColumnIndex(3);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.7f, 0.9f, 1.0f));
                ImGui::Text("%s", danhSachHS[i].email);
                ImGui::PopStyleColor();
                
                ImGui::TableSetColumnIndex(4);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.9f, 0.3f, 1.0f));
                ImGui::Text("%.2f", danhSachHS[i].gpa);
                ImGui::PopStyleColor();
                
                ImGui::TableSetColumnIndex(5);
                ImVec4 badgeColor = danhSachHS[i].getMauXepLoai();
                ImGui::PushStyleColor(ImGuiCol_Button, badgeColor);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, badgeColor);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, badgeColor);
                ImGui::SmallButton(danhSachHS[i].xepLoai);
                ImGui::PopStyleColor(3);
                
                ImGui::TableSetColumnIndex(6);
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.8f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
                char buttonLabel[32];
                sprintf(buttonLabel, "##delete%d", i);
                if (ImGui::SmallButton(buttonLabel)) {
                    deleteStudentIndex = i;
                    showDeleteConfirm = true;
                }
                ImGui::PopStyleColor(3);
                
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Xóa sinh viên");
                }
            }
            
            ImGui::EndTable();
        }
        
        ImGui::EndChild();
        
        ImGui::EndChild();
        ImGui::End();
    }
    
    void renderDeleteConfirmDialog() {
        if (!showDeleteConfirm || deleteStudentIndex < 0 || deleteStudentIndex >= danhSachHS.size())
            return;
        
        ImGui::SetNextWindowSize(ImVec2(500, 200), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), 
                                ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        
        ImGui::Begin("Xác Nhận Xóa", &showDeleteConfirm, 
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.7f, 0.2f, 1.0f));
        ImGui::TextWrapped("Bạn có chắc chắn muốn xóa sinh viên này?");
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        HocSinh& hs = danhSachHS[deleteStudentIndex];
        ImGui::Text("Họ tên: %s", hs.hoTen);
        ImGui::Text("Mã SV: %s", hs.maSV);
        ImGui::Text("GPA: %.2f", hs.gpa);
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
        ImGui::TextWrapped("Hành động này không thể hoàn tác!");
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        // Buttons
        float buttonWidth = 200.0f;
        float availWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX((availWidth - buttonWidth * 2 - 20) * 0.5f);
        
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("XÓA", ImVec2(buttonWidth, 40))) {
            danhSachHS.erase(danhSachHS.begin() + deleteStudentIndex);
            capNhatThongKe();
            
            if (selectedStudent == deleteStudentIndex) {
                selectedStudent = -1;
                showDetailDialog = false;
            } else if (selectedStudent > deleteStudentIndex) {
                selectedStudent--;
            }
            
            showDeleteConfirm = false;
            deleteStudentIndex = -1;
        }
        ImGui::PopStyleColor(3);
        
        ImGui::SameLine(0, 20);
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        if (ImGui::Button("HỦY", ImVec2(buttonWidth, 40))) {
            showDeleteConfirm = false;
            deleteStudentIndex = -1;
        }
        ImGui::PopStyleColor(3);
        
        ImGui::PopStyleVar();
        
        ImGui::End();
    }
    
    void renderAddDialog() {
        if (!showAddDialog) return;
        
        ImGui::SetNextWindowSize(ImVec2(800, 650), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), 
                                ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        
        ImGui::Begin("Thêm Sinh Viên Mới", &showAddDialog, ImGuiWindowFlags_NoCollapse);
        
        // Student info section
        ImGui::BeginChild("InfoSection", ImVec2(0, 180), true);
        ImGui::Text("THÔNG TIN CƠ BẢN");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Họ và Tên:");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##hoTen", inputHoTen, 100);
        
        ImGui::Columns(2, "BasicInfo", false);
        ImGui::Text("Mã Sinh Viên:");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##maSV", inputMaSV, 20);
        
        ImGui::NextColumn();
        ImGui::Text("Email:");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##email", inputEmail, 100);
        ImGui::Columns(1);
        
        ImGui::EndChild();
        
        ImGui::Spacing();
        
        // Add course section
        ImGui::BeginChild("CourseSection", ImVec2(0, 250), true);
        ImGui::Text("THÊM MÔN HỌC");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Tên Môn Học:");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##tenMon", inputTenMon, 100);
        
        ImGui::Columns(2, "CourseInfo", false);
        ImGui::Text("Điểm (0-10):");
        ImGui::SetNextItemWidth(-1);
        ImGui::SliderFloat("##diem", &inputDiem, 0.0f, 10.0f, "%.1f");
        
        ImGui::NextColumn();
        ImGui::Text("Số Tín Chỉ:");
        ImGui::SetNextItemWidth(-1);
        ImGui::SliderInt("##tinChi", &inputTinChi, 1, 5);
        ImGui::Columns(1);
        
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.7f, 1.0f, 1.0f));
        if (ImGui::Button("Thêm Môn Học", ImVec2(-1, 35))) {
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
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
        ImGui::Text("Đã thêm %d môn học", (int)tempMonHoc.size());
        
        ImGui::EndChild();
        
        // Course list
        if (!tempMonHoc.empty()) {
            ImGui::Spacing();
            ImGui::BeginChild("CourseList", ImVec2(0, 150), true);
            
            if (ImGui::BeginTable("TempCourses", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Môn Học");
                ImGui::TableSetupColumn("Điểm", ImGuiTableColumnFlags_WidthFixed, 60);
                ImGui::TableSetupColumn("Chữ", ImGuiTableColumnFlags_WidthFixed, 50);
                ImGui::TableSetupColumn("GPA", ImGuiTableColumnFlags_WidthFixed, 60);
                ImGui::TableSetupColumn("TC", ImGuiTableColumnFlags_WidthFixed, 50);
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
            
            ImGui::EndChild();
        }
        
        ImGui::Spacing();
        
        // Action buttons
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.9f, 0.4f, 1.0f));
        if (ImGui::Button("LƯU SINH VIÊN", ImVec2(200, 40))) {
            if (strlen(inputHoTen) > 0 && strlen(inputMaSV) > 0 && !tempMonHoc.empty()) {
                HocSinh hs;
                strcpy(hs.hoTen, inputHoTen);
                strcpy(hs.maSV, inputMaSV);
                strcpy(hs.email, inputEmail);
                hs.danhSachMon = tempMonHoc;
                hs.tinhGPA();
                
                danhSachHS.push_back(hs);
                capNhatThongKe();
                
                strcpy(inputHoTen, "");
                strcpy(inputMaSV, "");
                strcpy(inputEmail, "");
                tempMonHoc.clear();
                
                showAddDialog = false;
            }
        }
        ImGui::PopStyleColor(2);
        
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.4f, 0.4f, 1.0f));
        if (ImGui::Button("HỦY BỎ", ImVec2(200, 40))) {
            showAddDialog = false;
            tempMonHoc.clear();
        }
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
        
        // Delete button for detail dialog
        if (selectedStudent >= 0 && selectedStudent < danhSachHS.size()) {
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.8f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
            if (ImGui::Button("XÓA SV", ImVec2(150, 40))) {
                deleteStudentIndex = selectedStudent;
                showDeleteConfirm = true;
            }
            ImGui::PopStyleColor(2);
        }
        
        ImGui::End();
    }
    
    void renderDetailDialog() {
        if (!showDetailDialog || selectedStudent < 0 || selectedStudent >= danhSachHS.size()) 
            return;
        
        ImGui::SetNextWindowSize(ImVec2(850, 600), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), 
                                ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        
        ImGui::Begin("Chi Tiết Sinh Viên", &showDetailDialog, ImGuiWindowFlags_NoCollapse);
        
        HocSinh& hs = danhSachHS[selectedStudent];
        
        // Header card
        ImGui::BeginChild("HeaderCard", ImVec2(0, 180), true);
        
        ImVec2 headerPos = ImGui::GetCursorScreenPos();
        drawGradientRect(headerPos, ImVec2(ImGui::GetContentRegionAvail().x, 160),
                        IM_COL32(41, 98, 255, 200), IM_COL32(20, 50, 150, 200));
        
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
        ImGui::SetCursorPosX(30);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::Text("%s", hs.hoTen);
        ImGui::PopStyleColor();
        
        ImGui::SetCursorPosX(30);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
        ImGui::Text("Mã SV: %s", hs.maSV);
        ImGui::SetCursorPosX(30);
        ImGui::Text("Email: %s", hs.email);
        ImGui::PopStyleColor();
        
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        ImGui::Columns(2, "GPAInfo", false);
        ImGui::SetCursorPosX(30);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.9f, 0.3f, 1.0f));
        ImGui::Text("GPA: %.2f / 4.0", hs.gpa);
        ImGui::PopStyleColor();
        
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, hs.getMauXepLoai());
        ImGui::Text("Xếp loại: %s", hs.xepLoai);
        ImGui::PopStyleColor();
        ImGui::Columns(1);
        
        ImGui::EndChild();
        
        ImGui::Spacing();
        
        // Course statistics
        int tongTinChi = 0;
        float diemCaoNhat = 0.0f;
        float diemThapNhat = 10.0f;
        for (const auto& mon : hs.danhSachMon) {
            tongTinChi += mon.soTinChi;
            diemCaoNhat = max(diemCaoNhat, mon.diemSo);
            diemThapNhat = min(diemThapNhat, mon.diemSo);
        }
        
        ImGui::Columns(3, "CourseStats", false);
        ImGui::BeginChild("Stat1", ImVec2(0, 80), true);
        ImGui::SetCursorPosY(15);
        ImGui::SetCursorPosX(15);
        ImGui::Text("Tổng môn");
        ImGui::SetCursorPosX(15);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.8f, 1.0f, 1.0f));
        ImGui::Text("%d môn", (int)hs.danhSachMon.size());
        ImGui::PopStyleColor();
        ImGui::EndChild();
        
        ImGui::NextColumn();
        ImGui::BeginChild("Stat2", ImVec2(0, 80), true);
        ImGui::SetCursorPosY(15);
        ImGui::SetCursorPosX(15);
        ImGui::Text("Tổng tín chỉ");
        ImGui::SetCursorPosX(15);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 1.0f, 0.6f, 1.0f));
        ImGui::Text("%d TC", tongTinChi);
        ImGui::PopStyleColor();
        ImGui::EndChild();
        
        ImGui::NextColumn();
        ImGui::BeginChild("Stat3", ImVec2(0, 80), true);
        ImGui::SetCursorPosY(15);
        ImGui::SetCursorPosX(15);
        ImGui::Text("Điểm cao/thấp");
        ImGui::SetCursorPosX(15);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
        ImGui::Text("%.1f / %.1f", diemCaoNhat, diemThapNhat);
        ImGui::PopStyleColor();
        ImGui::EndChild();
        
        ImGui::Columns(1);
        ImGui::Spacing();
        
        // Course table
        ImGui::Text("BẢNG ĐIỂM CHI TIẾT");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::BeginChild("CourseTable", ImVec2(0, 0), true);
        
        if (ImGui::BeginTable("DetailTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("STT", ImGuiTableColumnFlags_WidthFixed, 50);
            ImGui::TableSetupColumn("Môn Học", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Điểm Số", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableSetupColumn("Điểm Chữ", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableSetupColumn("GPA", ImGuiTableColumnFlags_WidthFixed, 70);
            ImGui::TableSetupColumn("Tín Chỉ", ImGuiTableColumnFlags_WidthFixed, 70);
            ImGui::TableHeadersRow();
            
            for (int i = 0; i < hs.danhSachMon.size(); i++) {
                const auto& mon = hs.danhSachMon[i];
                ImGui::TableNextRow();
                
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", i + 1);
                
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", mon.tenMon);
                
                ImGui::TableSetColumnIndex(2);
                ImVec4 diemColor = mon.diemSo >= 8.0f ? ImVec4(0.2f, 0.9f, 0.4f, 1.0f) :
                                  mon.diemSo >= 6.5f ? ImVec4(1.0f, 0.8f, 0.2f, 1.0f) :
                                  mon.diemSo >= 5.0f ? ImVec4(1.0f, 0.6f, 0.2f, 1.0f) :
                                                       ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Text, diemColor);
                ImGui::Text("%.1f", mon.diemSo);
                ImGui::PopStyleColor();
                
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", mon.diemChu);
                
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%.1f", mon.diemGPA);
                
                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%d", mon.soTinChi);
            }
            
            ImGui::EndTable();
        }
        
        ImGui::EndChild();
        
        ImGui::Spacing();
        
        // Delete button at bottom
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("XÓA SINH VIÊN NÀY", ImVec2(-1, 45))) {
            deleteStudentIndex = selectedStudent;
            showDeleteConfirm = true;
        }
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();
        
        ImGui::End();
    }
    
    void renderStatsDialog() {
        if (!showStatsDialog) return;
        
        ImGui::SetNextWindowSize(ImVec2(700, 650), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), 
                                ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        
        ImGui::Begin("Thống Kê Tổng Quan", &showStatsDialog, ImGuiWindowFlags_NoCollapse);
        
        capNhatThongKe();
        
        // Overview cards
        ImGui::BeginChild("OverviewCards", ImVec2(0, 250), true);
        ImGui::Text("THỐNG KÊ TỔNG QUAN");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Columns(2, "StatsOverview", false);
        
        ImGui::Text("Tổng sinh viên:");
        ImGui::SameLine(200);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.8f, 1.0f, 1.0f));
        ImGui::Text("%d", stats.tongSV);
        ImGui::PopStyleColor();
        
        ImGui::Text("GPA trung bình:");
        ImGui::SameLine(200);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 1.0f, 0.6f, 1.0f));
        ImGui::Text("%.2f / 4.0", stats.gpaAvg);
        ImGui::PopStyleColor();
        
        ImGui::Text("GPA cao nhất:");
        ImGui::SameLine(200);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
        ImGui::Text("%.2f", stats.gpaMax);
        ImGui::PopStyleColor();
        
        ImGui::NextColumn();
        
        ImGui::Text("GPA thấp nhất:");
        ImGui::SameLine(200);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.3f, 1.0f));
        ImGui::Text("%.2f", stats.gpaMin);
        ImGui::PopStyleColor();
        
        ImGui::Text("Sinh viên xuất sắc:");
        ImGui::SameLine(200);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.8f, 1.0f));
        ImGui::Text("%d", stats.xuatSac);
        ImGui::PopStyleColor();
        
        ImGui::Text("Tỷ lệ đạt:");
        ImGui::SameLine(200);
        float tyLeDat = stats.tongSV > 0 ? (stats.tongSV - stats.yeu) * 100.0f / stats.tongSV : 0;
        ImGui::PushStyleColor(ImGuiCol_Text, tyLeDat >= 80 ? ImVec4(0.2f, 0.9f, 0.4f, 1.0f) : ImVec4(1.0f, 0.6f, 0.2f, 1.0f));
        ImGui::Text("%.1f%%", tyLeDat);
        ImGui::PopStyleColor();
        
        ImGui::Columns(1);
        
        ImGui::EndChild();
        
        ImGui::Spacing();
        
        // Classification distribution
        ImGui::BeginChild("Distribution", ImVec2(0, 0), true);
        ImGui::Text("PHÂN BỐ XẾP LOẠI HỌC LỰC");
        ImGui::Separator();
        ImGui::Spacing();
        
        if (stats.tongSV > 0) {
            float maxVal = max({(float)stats.xuatSac, (float)stats.gioi, (float)stats.kha, 
                               (float)stats.trungBinh, (float)stats.yeu});
            
            // Xuất sắc
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2f, 0.9f, 0.4f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.9f, 0.4f, 0.2f));
            ImGui::ProgressBar(stats.xuatSac / maxVal, ImVec2(-1, 40));
            ImGui::PopStyleColor(2);
            ImGui::SameLine(0, 15);
            ImGui::Text("Xuất sắc: %d (%.1f%%)", stats.xuatSac, stats.xuatSac * 100.0f / stats.tongSV);
            
            ImGui::Spacing();
            
            // Giỏi
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.3f, 0.7f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.3f, 0.7f, 1.0f, 0.2f));
            ImGui::ProgressBar(stats.gioi / maxVal, ImVec2(-1, 40));
            ImGui::PopStyleColor(2);
            ImGui::SameLine(0, 15);
            ImGui::Text("Giỏi: %d (%.1f%%)", stats.gioi, stats.gioi * 100.0f / stats.tongSV);
            
            ImGui::Spacing();
            
            // Khá
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.8f, 0.2f, 0.2f));
            ImGui::ProgressBar(stats.kha / maxVal, ImVec2(-1, 40));
            ImGui::PopStyleColor(2);
            ImGui::SameLine(0, 15);
            ImGui::Text("Khá: %d (%.1f%%)", stats.kha, stats.kha * 100.0f / stats.tongSV);
            
            ImGui::Spacing();
            
            // Trung bình
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 0.6f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.6f, 0.2f, 0.2f));
            ImGui::ProgressBar(stats.trungBinh / maxVal, ImVec2(-1, 40));
            ImGui::PopStyleColor(2);
            ImGui::SameLine(0, 15);
            ImGui::Text("Trung bình: %d (%.1f%%)", stats.trungBinh, stats.trungBinh * 100.0f / stats.tongSV);
            
            ImGui::Spacing();
            
            // Yếu
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.3f, 0.3f, 0.2f));
            ImGui::ProgressBar(stats.yeu / maxVal, ImVec2(-1, 40));
            ImGui::PopStyleColor(2);
            ImGui::SameLine(0, 15);
            ImGui::Text("Yếu: %d (%.1f%%)", stats.yeu, stats.yeu * 100.0f / stats.tongSV);
        } else {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Chưa có dữ liệu để hiển thị");
        }
        
        ImGui::EndChild();
        
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
        renderDeleteConfirmDialog();
    }
};

int main() {
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(1400, 850, "Quan Ly Diem GPA - Modern Edition", NULL, NULL);
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
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Load Vietnamese font
    ImFontConfig font_config;
    font_config.OversampleH = 2;
    font_config.OversampleV = 2;
    font_config.PixelSnapH = true;
    
    // Add default font with Vietnamese glyph ranges
    static const ImWchar ranges[] = {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0100, 0x024F, // Latin Extended-A + Latin Extended-B (Vietnamese characters)
        0x1E00, 0x1EFF, // Latin Extended Additional (Vietnamese)
        0,
    };
    
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 16.0f, &font_config, ranges);
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    QuanLyDiemGUI app;
    
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
        glClearColor(0.09f, 0.09f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
