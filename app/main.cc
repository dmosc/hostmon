#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

int main()
{
    ftxui::Element document = ftxui::vbox({ftxui::text("Hello world!") | ftxui::bold | ftxui::color(ftxui::Color::Cyan)}) | ftxui::border;
    auto screen = ftxui::Screen::Create(ftxui::Dimension::Fit(document));
    ftxui::Render(screen, document);
    screen.Print();
    return 0;
}