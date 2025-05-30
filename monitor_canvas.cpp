#include "monitor_canvas.hpp"
#include "catch_amalgamated.hpp"
#include <unistd.h>
#include <algorithm>
#include <wx/font.h>
#include <wx/gtk/bitmap.h>
#include <wx/dcgraph.h>

namespace system_monitor {
using std::min;

MonitorCanvas::MonitorCanvas(const wxString &title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1400, 800)),
      download_history_(network_history_length, 0.0),
      upload_history_(network_history_length, 0.0) {
  SetBackgroundStyle(wxBG_STYLE_PAINT);

  scroll_panel_ = new wxScrolledWindow(this);
  scroll_panel_->SetScrollRate(10, 10);
  scroll_panel_->SetBackgroundStyle(wxBG_STYLE_PAINT);
  scroll_panel_->SetScrollbars(0, 20, 0, 100, 0, 0, true);

  scroll_panel_->Bind(wxEVT_PAINT, &MonitorCanvas::on_paint, this);
  scroll_panel_->Bind(wxEVT_LEFT_DOWN, &MonitorCanvas::on_click, this);

  timer_ = new wxTimer(this);
  Bind(wxEVT_TIMER, &MonitorCanvas::on_timer, this);
  timer_->Start(500);

  cards_[0].label = "RAM";
  cards_[1].label = "Drive";
  cards_[2].label = "CPU";
  cards_[0].usage = monitor_.ram.get_usage();
  cards_[1].usage = monitor_.drive.get_usage();
  cards_[2].usage = monitor_.cpu.get_usage();
        }

    void MonitorCanvas::on_timer(wxTimerEvent&) {
        cards_[0].usage = monitor_.ram.get_usage();
        cards_[1].usage = monitor_.drive.get_usage();
        cards_[2].usage = monitor_.cpu.get_usage();

        double download = monitor_.network.get_download_rate() / 1024.0;
        double upload = monitor_.network.get_upload_rate() / 1024.0;
        update_network_histroy(download, upload);

        scroll_panel_->Refresh();
    }

    void MonitorCanvas::update_network_histroy(double download, double upload){
        download_history_[network_graph_index_] = download;
        upload_history_[network_graph_index_] = upload;
        network_graph_index_++;
        if(network_graph_index_ >= network_history_length) {
            network_graph_index_ = 0;
            network_graph_full_ = true;
        }
    }

    void MonitorCanvas::on_paint(wxPaintEvent&) {
        wxPaintDC dc(scroll_panel_);
        scroll_panel_->DoPrepareDC(dc);
        render(dc);
    }

    void MonitorCanvas::on_click(wxMouseEvent& event) {
        int x, y;
        scroll_panel_->CalcUnscrolledPosition(event.GetX(), event.GetY(), &x, &y);

        wxClientDC dc(scroll_panel_);
        scroll_panel_->DoPrepareDC(dc);

        for(int i = 0; i < n_cards; ++i){
            if(get_show_more_rect(cards_[i], dc).Contains(x, y)){
                cards_[i].expanded = !cards_[i].expanded;
                scroll_panel_->Refresh();
                break;
            }
        }
    }

    wxRect MonitorCanvas::get_show_more_rect(const Cards& card, wxDC& dc) const {
        wxFont font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        dc.SetFont(font);
        dc.SetTextForeground(wxColour(33, 150, 243));

        wxString text = card.expanded ? "show less" : "show more";

        int tw, th;
        dc.GetTextExtent(text, &tw, &th);

        int base_cardHeight = card.rect.height / (card.expanded ? 2 : 1);
        int center_x = card.rect.x + card.rect.width / 2;
        int center_y = card.rect.y + base_cardHeight / 2 - 10;
        int circle_size = min(card.rect.width, base_cardHeight) * 0.6;
        int circle_radius = circle_size / 2;
        int show_more_y = center_y + circle_radius + 18;

        return wxRect(center_x - tw / 2 - 4, show_more_y - 2, tw + 8, th + 4);
    }

    void MonitorCanvas::render(wxDC& dc) {
        int width, height;
        scroll_panel_->GetClientSize(&width, &height);

        int base_cardWidth = (width - (n_cards + 1) * spacing) / n_cards;
        int base_cardHeight = height / 2 - 2 * spacing;

        // Draw cards
        int x = spacing;
        int cards_bottom = 0;

        for(int i = 0; i < n_cards; ++i){
            cards_[i].rect = wxRect(x, spacing, base_cardWidth, cards_[i].expanded ? 2 * base_cardHeight : base_cardHeight);
            draw_card(dc, cards_[i], base_cardHeight);
            x += base_cardWidth + spacing;
            if(cards_[i].rect.GetBottom() > cards_bottom)
                cards_bottom = cards_[i].rect.GetBottom();
        }

        int info_y = cards_bottom + spacing;

        int section_width = (width - 3 * spacing) / 2;
        int section_height = (height / 2) - 2 * spacing;

        // General info Section
        draw_info_section(dc, spacing, info_y, section_width, section_height, true);

        // Network Section
        draw_info_section(dc, 2 * spacing + section_width, info_y, section_width, section_height, false);

        int scroll_height = info_y + section_height + spacing;
        scroll_panel_->SetVirtualSize(wxSize(width, scroll_height));
    }

    void MonitorCanvas::draw_card(wxDC& dc, Cards& card, int base_cardHeight) {
        // Draw rounded rectangle (card background)
        wxColour card_bg(255, 255, 255);
        wxColour card_border(180, 180, 180);
        dc.SetBrush(wxBrush(card_bg));
        dc.SetPen(wxPen(card_border, 2));
        const int corner_radius = 20;
        dc.DrawRoundedRectangle(card.rect.x, card.rect.y, card.rect.width, card.rect.height, corner_radius);

        // Draw the usage circle
        int center_x = card.rect.x + card.rect.width / 2;
        int center_y = card.rect.y + base_cardHeight / 2 - 10;
        int circle_size = min(card.rect.width, base_cardHeight) * 0.6;
        int circle_radius = circle_size / 2;

        wxColour usage_col(76, 175, 80); // RAM
        if (card.label == "CPU") usage_col = wxColour(33, 150, 243); // CPU
        else if (card.label == "Drive") usage_col = wxColour(255, 152, 0); // Drive

        wxString usage_text = wxString::Format("%.1f%%", card.usage * 100.0);

        draw_usage_circle(dc, center_x, center_y, circle_radius, card.usage, usage_col, usage_text);

        draw_title(dc, card.rect.x, card.rect.y, card.label, card.rect.width);
        int show_more_y = center_y + circle_radius + 18;
        draw_show_more_text(dc, center_x, show_more_y, card.expanded);

        if(card.expanded) {
            int info_y = show_more_y + 80;
            int info_x = card.rect.x + 30;
            if(card.label == "RAM")
                draw_ram_info(dc, card, info_x, info_y);
            if(card.label == "Drive")
                draw_drive_info(dc, card, info_x, info_y);
            if(card.label == "CPU")
                draw_cpu_info(dc, card, info_x, info_y);
        }
    }

    // draws sections at the bottom
    void MonitorCanvas::draw_info_section(wxDC& dc, int x, int y, int w, int h, bool is_general) {
        wxColour card_bg(255, 255, 255);
        wxColour card_border(180, 180, 180);
        dc.SetBrush(wxBrush(card_bg));
        dc.SetPen(wxPen(card_border, 2));
        const int corner_radius = 20;
        dc.DrawRoundedRectangle(x, y, w, h + spacing, corner_radius);

        if(is_general) {
            draw_system_infos(dc, x + spacing, y + spacing);
        } else {
            draw_network_infos(dc, x + spacing, y + spacing, w);
        }
    }

    // draws the usage circles of components
    void MonitorCanvas::draw_usage_circle(wxDC& dc, int center_x, int center_y, int radius, double usage, const wxColour& color, const wxString& usage_text) {
        wxColour bg_circle(220, 220, 220);
        dc.SetPen(wxPen(bg_circle, 10));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawEllipse(center_x - radius, center_y - radius, 2 * radius, 2 * radius);

        dc.SetPen(wxPen(color, 10));
        double start_angle = -90.0; // top
        double end_angle = start_angle + usage * 360.0;

        if (usage > 0.0) {
            if (usage >= 1.0) usage = 0.999; // DrawEllipticArc does not allow values over 0.999
            dc.DrawEllipticArc(center_x - radius, center_y - radius, 2 * radius, 2 * radius, start_angle, end_angle);
        }
        draw_percentage_text(dc, center_x, center_y, usage_text);
    }

    // draws title of each card
    void MonitorCanvas::draw_title(wxDC& dc, int x, int y, const wxString& label, int box_width) {
        wxFont font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        dc.SetFont(font);
        dc.SetTextForeground(*wxBLACK);

        int tw, th;
        dc.GetTextExtent(label, &tw, &th);

        int label_x = x + (box_width - tw) / 2;
        int label_y = y +  18;

        dc.DrawText(label, label_x, label_y);
    }

    // draws percentag text in center of usage circle
    void MonitorCanvas::draw_percentage_text(wxDC& dc, int center_x, int center_y, const wxString& usage_text) {
        wxFont font(percent_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        dc.SetFont(font);
        dc.SetTextForeground(*wxBLACK);

        int tw, th;
        dc.GetTextExtent(usage_text, &tw, &th);
        dc.DrawText(usage_text, center_x - tw / 2, center_y - th / 2);
    }

    // draws show more "button"
    void MonitorCanvas::draw_show_more_text(wxDC& dc, int center_x, int y, bool expanded) {
        wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        dc.SetFont(font);

        wxString text = expanded ? "show less" : "show more";

        int tw, th;
        dc.GetTextExtent(text, &tw, &th);

        int button_width = tw + 20;
        int button_height = th + 20;
        int button_x = center_x - button_width / 2;

        dc.SetBrush(wxBrush(wxColor(230, 242, 255)));
        dc.SetPen(wxPen(wxColor(33, 150, 242), 2));
        dc.DrawRoundedRectangle(button_x, y, button_width, button_height, 8);

        dc.SetTextForeground(wxColour(33, 150, 243));
        dc.DrawText(text, center_x - tw / 2, y + 9);
    }

    void MonitorCanvas::draw_ram_info(wxDC& dc, const Cards&, int info_x, int info_y) {
        wxFont heading_font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont info_font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        dc.SetFont(heading_font);
        dc.SetTextForeground(*wxBLACK);

        dc.DrawText("RAM informations:", info_x, info_y);

        unsigned long long total = monitor_.ram.total();
        unsigned long long used = monitor_.ram.used();
        unsigned long long free = monitor_.ram.free();

        int line_y = info_y + 35;
        dc.SetFont(info_font);

        dc.DrawText(wxString::Format("Total memory: %.2f GiB", static_cast<double>(total) / (1024.0 * 1024 * 1024)), info_x, line_y);
        line_y += 25;
        dc.DrawText(wxString::Format("Free memory: %.2f GiB", static_cast<double>(used) / (1024.0 * 1024 * 1024)), info_x, line_y);
        line_y += 25;
        dc.DrawText(wxString::Format("Used memory: %.2f GiB", static_cast<double>(free) / (1024.0 * 1024 * 1024)), info_x, line_y);
    }

    void MonitorCanvas::draw_drive_info(wxDC& dc, const Cards&, int info_x, int info_y) {
        wxFont heading_font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont info_font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        dc.SetFont(heading_font);
        dc.SetTextForeground(*wxBLACK);

        dc.DrawText("Drive informations:", info_x, info_y);

        unsigned long long total = monitor_.drive.total();
        unsigned long long used = monitor_.drive.used();
        unsigned long long free = monitor_.drive.free();

        wxCoord line_y = info_y + 35;
        dc.SetFont(info_font);

        dc.DrawText(wxString::Format("Total memory: %.2f GiB", static_cast<double>(total) / (1024.0 * 1024 * 1024)), info_x, line_y);
        line_y += 25;
        dc.DrawText(wxString::Format("Free memory: %.2f GiB", static_cast<double>(used) / (1024.0 * 1024 * 1024)), info_x, line_y);
        line_y += 25;
        dc.DrawText(wxString::Format("Used memory: %.2f GiB", static_cast<double>(free) / (1024.0 * 1024 * 1024)), info_x, line_y);
    }

    void MonitorCanvas::draw_cpu_info(wxDC& dc, const Cards&, int info_x, int info_y) {
        wxFont heading_font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont info_font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        dc.SetFont(heading_font);
        dc.SetTextForeground(*wxBLACK);

        dc.DrawText("CPU informations:", info_x, info_y);
        int line_y = info_y + 28;

        dc.SetFont(info_font);
        wxString info_text = wxString::Format("Further Informations about CPU need to \nbe implemented.");
        dc.DrawText(info_text, info_x, line_y);
    }

    void MonitorCanvas::draw_system_infos(wxDC& dc, int info_x, int info_y) {
        wxFont heading_font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont subheading_font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont info_font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        dc.SetFont(heading_font);
        dc.SetTextForeground(*wxBLACK);

        dc.DrawText("General informations:", info_x, info_y);

        unsigned int core_num = monitor_.general.get_cpu_cores();
        wxString model_name = monitor_.general.get_cpu_model();
        wxString product_name = monitor_.general.get_product_name();
        wxString kde_version = monitor_.general.get_os_version();
        wxString kernel_version = monitor_.general.get_kernel_version();

        unsigned long uptime = monitor_.general.get_uptime();
        unsigned long procs_num = monitor_.general.get_procs_num();

        int line_y = info_y + 40;

        dc.SetFont(info_font);

        wxString cpus = wxString::Format("Processors: %u x " + model_name, core_num);
        wxString product_text = wxString::Format("Productname: " + product_name);
        wxString os_version_text = wxString::Format("KDE-Plasma-Version: " + kde_version);;
        wxString kernel_text = wxString::Format("Kernel-Version: " + kernel_version);
        wxString uptime_text = wxString::Format("System uptime since boot (seconds): %llu", uptime);
        wxString procs_text = wxString::Format("Number of processes running: %llu", procs_num);
        dc.SetFont(subheading_font);
        dc.DrawText("Hardware:", info_x , line_y);
        dc.SetFont(info_font);
        line_y += spacing;
        dc.DrawText(cpus, info_x, line_y);
        line_y += spacing;
        dc.DrawText(product_text, info_x, line_y);
        line_y += spacing + 2;
        dc.SetFont(subheading_font);
        dc.DrawText("Software:", info_x, line_y);
        dc.SetFont(info_font);
        line_y += spacing;
        dc.DrawText(os_version_text, info_x, line_y);
        line_y += spacing;
        dc.DrawText(kernel_text, info_x, line_y);
        line_y += spacing + 2;
        dc.SetFont(subheading_font);
        dc.DrawText("Other:", info_x, line_y);
        dc.SetFont(info_font);
        line_y += spacing;
        dc.DrawText(uptime_text, info_x, line_y);
        line_y += spacing;
        dc.DrawText(procs_text, info_x, line_y);
    }

    void MonitorCanvas::draw_network_infos(wxDC& dc, int info_x, int info_y, int width) {
        wxFont heading_font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont subheading_font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont info_font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        dc.SetFont(heading_font);
        dc.SetTextForeground(*wxBLACK);

        double download_rate = monitor_.network.get_download_rate();
        double upload_rate = monitor_.network.get_upload_rate();

        wxString dowload_text = wxString::Format("Download: %.1f KiB/s", download_rate);
        wxString upload_text = wxString::Format("Upload: under construction");

        int download_text_width, download_text_height;
        dc.GetTextExtent(dowload_text, &download_text_width, &download_text_height);

        dc.DrawText("Network informations:", info_x, info_y);
        dc.SetFont(subheading_font);

        int line_y = info_y + 40;

        dc.DrawText(dowload_text, info_x, line_y);
        dc.DrawText(upload_text, info_x + 10 * spacing, line_y);
        line_y += spacing;

        // Network Graph
        int graph_x = info_x;
        int graph_y = line_y;
        int graph_width = width - 2 * spacing;
        int graph_height = 200;
        draw_network_graph(dc, graph_x, graph_y, graph_width, graph_height);
    }

    void MonitorCanvas::draw_network_graph(wxDC& dc, int x, int y, int w, int h){
        // Background square
        dc.SetPen(wxPen(wxColour(50, 50, 60)));
        dc.SetBrush(wxBrush(wxColour(35, 35, 45)));
        dc.DrawRectangle(x, y, w, h);

        // Lines
        dc.SetPen(wxPen(wxColour(60, 60, 80)));
        for(int i = 1; i < 5; ++i){
            int yline = y + h * i / 5;
            dc.DrawLine(x, yline, x + w, yline);
        }

        double max_val = 0.0;
        size_t points = network_graph_full_ ? network_history_length :network_graph_index_;
        for(size_t i = 0; i < points; ++i) {
            max_val = std::max({max_val, download_history_[i], upload_history_[i]});
        }
        if(max_val < 1e-6) max_val = 1.0;

        // Download Line (green)
        dc.SetPen(wxPen(wxColour(80, 220, 60), 2));
        for(size_t i = 1; i < points; ++i) {
            int idx0 = (network_graph_index_ + i - 1) % network_history_length;
            int idx1 = (network_graph_index_ + i) % network_history_length;
            int x0 = x + (w * (i - 1)) / (network_history_length - 1);
            int x1 = x + (w * i) / (network_history_length - 1);
            int y0 = y + h - int(h * std::min(download_history_[idx0] /max_val, 1.0));
            int y1 = y + h - int(h * std::min(download_history_[idx1] /max_val, 1.0));
            dc.DrawLine(x0, y0, x1, y1);
        }

        dc.SetTextForeground(*wxWHITE);
        dc.DrawText(wxString::Format("%.1f MiB/s", max_val), x, y);
    }
    } // namespace system_monitor
