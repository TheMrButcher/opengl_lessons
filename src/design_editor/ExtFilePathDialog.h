#pragma once

#include <gamebase/engine/Panel.h>
#include <gamebase/engine/TextBox.h>
#include <gamebase/text/Conversion.h>

namespace gamebase {

class LinearLayout;
class ScrollableArea;
class StaticLabel;

namespace editor {

class ExtFilePathDialog {
public:
    ExtFilePathDialog() {}

    ExtFilePathDialog(Panel* panel);

    const std::string& rootPath() const { return m_rootPath; }
    void setRootPath(const std::string& value) { m_rootPath = value; }

    const std::string& relativePath() const { return m_relativePath; }
    void setRelativePath(const std::string& value) { m_relativePath = value; }

    const std::string& fileName() const { return m_textBox->text(); }
    void setFileName(const std::string& value) { m_textBox->setText(value); }

    void setCallbacks(
        const std::function<void(const std::string&, const std::string&)>& okCallback,
        const std::function<void()>& cancelCallback = nullptr)
    {
        m_panel->setCloseCallback(cancelCallback);
        m_cancel->setCallback(std::bind(&Panel::close, m_panel));
        m_ok->setCallback(std::bind(&ExtFilePathDialog::processResult, this, okCallback));
    }

    void init()
    {
        updateFilesView();
        m_panel->resetPosition();
        m_panel->setVisible(true);
    }

    void init(const std::function<void(const std::string&)>& okCallback)
    {
        std::function<void(const std::string&, const std::string&)> adapted =
            std::bind(&ExtFilePathDialog::adaptCall, this, okCallback,
                std::placeholders::_1, std::placeholders::_2);
        setCallbacks(adapted);
        init();
    }

private:
    void updateFilesView();
    void goUp();
    void goInto(const std::string& dirName);
    void selectFile(const std::string& fileName);

    void adaptCall(
        const std::function<void(const std::string&)>& okCallback,
        const std::string& path,
        const std::string& fileName);
    void processResult(const std::function<void(const std::string&, const std::string&)>& callback);

    Panel* m_panel;

    LinearLayout* m_filesList;
    ScrollableArea* m_filesArea;
    TextBox* m_textBox;
    StaticLabel* m_absPathLabel;
    Button* m_ok;
    Button* m_cancel;
    std::string m_rootPath;
    std::string m_relativePath;
};

} }