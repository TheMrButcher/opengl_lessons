/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "ExtFilePathDialog.h"
#include "tools.h"
#include <gamebase/serial/LoadObj.h>
#include <gamebase/impl/ui/ScrollableArea.h>
#include <gamebase/text/StringUtils.h>
#include <gamebase/text/StringConversion.h>
#include <gamebase/tools/FileIO.h>

namespace gamebase { namespace editor {

namespace {
std::string formFullPath(
    const std::string& rootPath,
    const std::string& relativePath,
    const std::string& fileName)
{
    auto fullPath = addSlash(rootPath);
    if (fileName.empty())
        fullPath += relativePath;
    else
        fullPath += addSlash(relativePath) + fileName;
    return fullPath;
}
}

void ExtFilePathDialog::attachPanel(Panel panel)
{
	m_panel = panel;
	m_filesList = panel.child<Layout>("filesList");
	m_filesArea = panel.child<Layout>("filesArea");
	m_textBox = panel.child<TextBox>("textbox");
	m_absPathLabel = panel.child<Label>("absPath");
	m_ok = panel.child<Button>("ok");
	m_cancel = panel.child<Button>("cancel");
    m_panel.hide();
}

void ExtFilePathDialog::setFilePath(const std::string& value)
{
	if (value.empty())
		return;
	setRelativePath(pathToDir(value));
	setFileName(gamebase::fileName(value));
}

void ExtFilePathDialog::updateFilesView()
{
    m_filesList.clear();
	try {
		auto fullPath = addSlash(m_rootPath) + m_relativePath;
		if (fullPath.empty())
			fullPath = ".";
		auto fullPathLocal = toLocal(fullPath);
		auto absPathLocal = normalizePath(absolutePath(addSlash(fullPathLocal)));
		m_absPathLabel.setText(toUnicode(absPathLocal));
		if (absPathLocal.length() > 3) {
			auto button = loadObj<Button>("ui\\DirButton.json");
			button.child<Label>("label").setText("..");
			button.setCallback([this]() { goUp(); });
			m_filesList.add(button);
		}

		auto files = listFilesInDirectory(fullPathLocal);
		for (auto it = files.begin(); it != files.end(); ++it) {
			const auto& desc = *it;
			if (desc.type == FileDesc::Directory)
			{
				auto dirNameUtf8 = toUnicode(desc.fileName);
				auto button = loadObj<Button>("ui\\DirButton.json");
				button.child<Label>("label").setText(dirNameUtf8);
				button.setCallback([this, dirNameUtf8]() { goInto(dirNameUtf8); });
				m_filesList.add(button);
			}
		}
        
		for (auto it = files.begin(); it != files.end(); ++it) {
			const auto& desc = *it;
			if (desc.type == FileDesc::File)
			{
				auto fullFileNameUtf8 = toUnicode(desc.fullName());
				auto button = loadObj<Button>("ui\\FileButton.json");
				button.child<Label>("label").setText(fullFileNameUtf8);
				button.setCallback([this, fullFileNameUtf8]() { selectFile(fullFileNameUtf8); });
				m_filesList.add(button);
			}
		}

		m_filesArea.update();
	} catch (Exception& ex) {
		if (m_relativePath.empty()) {
			m_panel.hide();
			showError("Error while showing file dialog", ex.what());
			return;
		}

		m_relativePath = "";
		updateFilesView();
	}
}

void ExtFilePathDialog::goUp()
{
    m_relativePath = addSlash(m_relativePath) + "..";
    updateFilesView();
}

void ExtFilePathDialog::goInto(const std::string& dirName)
{
    m_relativePath = addSlash(m_relativePath) + dirName;
    updateFilesView();
}

void ExtFilePathDialog::selectFile(const std::string& fileName)
{
    m_textBox.setText(fileName);
}

void ExtFilePathDialog::adaptCall(
    const std::function<void(const std::string&)>& okCallback,
    const std::string& path,
    const std::string& fileName)
{
    if (okCallback) {
        auto fullPath = formFullPath(m_rootPath, path, fileName);
        okCallback(fullPath);
    }
}

void ExtFilePathDialog::processResult(
    const std::function<void(const std::string&, const std::string&)>& callback)
{
    auto rootPathLocal = toLocal(m_rootPath);
    auto relativePathLocal = toLocal(m_relativePath);
    auto fileNameLocal = toLocal(fileName());
    auto pathToDirLocal = normalizePath(formFullPath(rootPathLocal, relativePathLocal, ""));
    relativePathLocal = gamebase::relativePath(rootPathLocal, pathToDirLocal);
    auto fullPathLocal = normalizePath(formFullPath(
        rootPathLocal, relativePathLocal, fileNameLocal));

    if (m_config.mode == Config::Save) {
        bool isSameFile = false;
        if (m_config.curFilePathLocal)
            isSameFile = *m_config.curFilePathLocal == fullPathLocal;

        bool isExistingFile = fileExists(fullPathLocal);

        if (!isSameFile && isExistingFile) {
            getConfirmationDialog().init("overwrite", [this, callback, relativePathLocal, fileNameLocal]()
            {
                if (callback)
                    callback(relativePathLocal, fileNameLocal);
                m_panel.hide();
            });
            return;
        }
    }
    if (callback)
        callback(relativePathLocal, fileNameLocal);
    m_panel.hide();
}

} }
