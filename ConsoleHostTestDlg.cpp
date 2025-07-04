﻿
// ConsoleHostTestDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ConsoleHostTest.h"
#include "ConsoleHostTestDlg.h"
#include "afxdialogex.h"
#include "httplib.h"

#include "json.hpp"
using json = nlohmann::json;

#include <string>
#include <codecvt> // codecvt_utf8
#include <locale>  // wstring_convert

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CConsoleHostTestDlg 对话框



CConsoleHostTestDlg::CConsoleHostTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONSOLEHOSTTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CConsoleHostTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_USER_INPUT, m_edtInput);
	DDX_Control(pDX, IDC_RICHEDIT22_CHAT, m_edtOutput);
}

BEGIN_MESSAGE_MAP(CConsoleHostTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_USER_SEND, &CConsoleHostTestDlg::OnBnClickedButtonUserSend)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CConsoleHostTestDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_TEST_REMOTE, &CConsoleHostTestDlg::OnBnClickedButtonTestRemote)
END_MESSAGE_MAP()


// CConsoleHostTestDlg 消息处理程序

BOOL CConsoleHostTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 打开控制台窗口
	if (::GetConsoleWindow() == NULL)
	{
		if (::AllocConsole())
		{
			FILE* stream;
			freopen_s(&stream, "CONOUT$", "w", stdout);
		}
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CConsoleHostTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CConsoleHostTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CConsoleHostTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CConsoleHostTestDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
}

void CConsoleHostTestDlg::OnOK()
{
	OnBnClickedButtonUserSend();

	// 注意：不要调用CDialog::OnOK()，否则对话框会关闭
	// CDialog::OnOK();
}


// https://json.nlohmann.me/home/faq/#wide-string-handling
// encoding function
std::string to_utf8(std::wstring& wide_string)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
	return utf8_conv.to_bytes(wide_string);
}

#pragma warning(disable : 4996)
std::wstring ANSIToUnicode(const std::string& str)
{
	std::wstring ret;
	std::mbstate_t state = {};
	const char* src = str.data();
	size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
	if (static_cast<size_t>(-1) != len) {
		std::unique_ptr< wchar_t[] > buff(new wchar_t[len + 1]);
		len = std::mbsrtowcs(buff.get(), &src, len, &state);
		if (static_cast<size_t>(-1) != len) {
			ret.assign(buff.get(), len);
		}
	}
	return ret;
}

//#define _TEST_LOCALHOST
void CConsoleHostTestDlg::OnBnClickedButtonUserSend()
{
	CString strInput;
	m_edtInput.GetWindowText(strInput);

	if (!strInput.IsEmpty()) {
		// 显示输入命令
		m_edtOutput.SetSel(-1, -1);
		m_edtOutput.ReplaceSel(_T("\n\n> ") + strInput + _T("\r\n"));

		// 发送到Ollama
		json jsRequest;
		jsRequest["model"] = "deepseek-r1:8b";
		std::wstring wsInput = std::wstring((LPCTSTR)strInput);
		jsRequest["prompt"] = to_utf8(wsInput);
		jsRequest["stream"] = true;
		std::string strRequest = jsRequest.dump(4);
		std::cout << strRequest << std::endl;

#ifdef _TEST_LOCALHOST
		httplib::Client cli("localhost", 11434);
#else
		httplib::Client cli("10.65.6.70", 11434);
#endif
		auto res = cli.Post("/api/generate", strRequest, "application/json");
		if (res && res->status == 200) {
#ifdef _TEST_LOCALHOST
			std::cout << "LLM replies: " << res->body << std::endl;
#endif	
			// 流式响应处理
			std::istringstream iss(res->body);
			std::string line;
			while (std::getline(iss, line)) { // 使用独立线程处理才会有效果哦！
				if (!line.empty()) {
					// 解析JSON行并提取内容
					json jsResponse = json::parse(line);
					m_edtOutput.SetSel(-1, -1); // 滚动到末尾
					m_edtOutput.ReplaceSel(ANSIToUnicode(jsResponse["response"]).c_str()); // 添加输出

					//std::cout << "New Line: " << line << std::endl;
				}
			}
		}
		else {
			std::cout << "Failed to get a response from LLM";
			if (res) {
				std::cout << "Status code: " << res->status << std::endl;
			}
		}

		m_edtInput.SetWindowText(_T(""));
	}
}

void CConsoleHostTestDlg::OnBnClickedButtonTest()
{
	httplib::Client cli("localhost", 11434);

	// 构建请求JSON
	std::string request = R"(
    {
        "model": "llama2",
        "prompt": "你好，请介绍一下自己。",
        "stream": false
    }
    )";

	// 调用Ollama API
	auto res = cli.Post("/api/generate", request, "application/json");

	if (res && res->status == 200) {
		std::cout << "模型回复：" << res->body << std::endl;
	}
	else {
		std::cout << "请求失败!";
		if (res) {
			std::cout << "状态码：" << res->status << std::endl;
		}		
	}
}

/*
* 配置Ollama服务，使其在局域网内可被其他机器访问
* 1. ipconfig获取本机IP地址（e.g. 10.65.6.70）
* 2. 在系统环境变量里设置OLLAMA_HOST，值为http://10.65.6.70:11434 
* 3. 重启电脑。在浏览器里测试上述IP，如弹出防火墙，则允许
* 
* ollama -h 列出所有命令
* ollama list 列出所有本地模型
* ollama serve -h 可以列出所有支持的环境变量
*/
void CConsoleHostTestDlg::OnBnClickedButtonTestRemote()
{
	httplib::Client cli("10.65.6.70", 11434);

	// 构建请求JSON
	std::string request = R"(
    {
        "model": "deepseek-r1:8b",
        "prompt": "who is the president of USA",
        "stream": false
    }
    )";

	// 调用Ollama API
	auto res = cli.Post("/api/generate", request, "application/json");

	if (res && res->status == 200) {
		std::cout << "模型回复：" << res->body << std::endl;
	}
	else {
		std::cout << "请求失败!";
		if (res) {
			std::cout << "状态码：" << res->status << std::endl;
		}
	}
}
