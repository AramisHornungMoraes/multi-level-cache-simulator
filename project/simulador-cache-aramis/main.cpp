// cache core
#include "cachedef.h"
#include "cachesim.h"


// interface
#include <irrlicht.h>
#include "driverChoice.h"

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

// simulation variables
int sok = 0; // simmulator is ok to begin?
int add_number = 32; // numero de enderecos
ender *enderecos_ui; // simulacao do que seria os enderecos que a cpu nos solicita
int end_index_ui = 0; // auxilia no termino da simulacao; ou seja, ate o numero de endereços solicitados
int add_range = 256;

struct SAppContext
{
	IrrlichtDevice *device;
	s32				counter;

	IGUIListBox *listbox;
	IGUIEditBox *cash_size_edit_box;
	IGUIEditBox *cash_line_size_edit_box;
	IGUIEditBox *cycle_size_edit_box;
	IGUIListBox *listboxfa;
	IGUIListBox *listboxdmi;
	IGUIListBox *listboxdmd;
	IGUIListBox *listbox2w;
	IGUIListBox *listbox4w;
	IGUIListBox *listbox8w;
	IGUIEditBox *listboxnumberadd;
	IGUIEditBox *listboxrange;
	IGUIEditBox *eb_filename;
	IGUICheckBox *cb_openfromfile;
	IGUICheckBox *fa_lru;
	IGUICheckBox *sa2w_lru;
	IGUICheckBox *sa4w_lru;
	IGUICheckBox *sa8w_lru;
	IGUIStaticText *debugInfo;
};

void update_informations(); // signature of the fuction below, help the Event receiver class knows its existence. -Yep its a hack. - Aramis Hornung Moraes 2013-11-15

// Define some values that we'll use to identify individual GUI controls.
enum
{
	GUI_ID_QUIT_BUTTON = 101,
	gui_id_open_settings_window,
	gui_id_viewfa,
	gui_id_viewdmi,
	gui_id_viewdmd,
	gui_id_view2w,
	gui_id_view4w,
	gui_id_view8w,
	gui_id_viewfaw,
	gui_id_viewdmiw,
	gui_id_viewdmdw,
	gui_id_view2ww,
	gui_id_view4ww,
	gui_id_view8ww,
	gui_id_nextclock,
	gui_id_nextcycle,
	gui_id_executeall,
	gui_id_settings_window,
	gui_id_console_window,
	gui_id_open_console_window,
	gui_id_open_general_window,
	gui_id_general_window,
	gui_id_fa_lru,
	gui_id_2w_lru,
	gui_id_4w_lru,
	gui_id_8w_lru,
	apply_cache_varaibles
};
IGUIEditBox *cash_size_edit_box;
IGUIEditBox *cash_line_size_edit_box;
IGUIEditBox *cycle_size_edit_box;
IGUIListBox *listbox; // log
IGUIListBox *listboxfa;
IGUIListBox *listboxdmi;
IGUIListBox *listboxdmd;
IGUIListBox *listbox2w;
IGUIListBox *listbox4w;
IGUIListBox *listbox8w;
IGUIEditBox *listboxnumberadd;
IGUIEditBox *listboxrange;
IGUIEditBox *eb_filename;
IGUICheckBox *cb_openfromfile;
IGUICheckBox *fa_lru;
IGUICheckBox *sa2w_lru;
IGUICheckBox *sa4w_lru;
IGUICheckBox *sa8w_lru;
IGUIStaticText *debugInfo;


class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context){}


	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			IGUIEnvironment* env = Context.device->getGUIEnvironment();

			switch(event.GUIEvent.EventType)
			{

			case EGET_BUTTON_CLICKED:
				switch(id)
				{
				case GUI_ID_QUIT_BUTTON:
					Context.device->closeDevice();
					return true;

					case apply_cache_varaibles: // CACHE DEFINITIONS
					{
						sok = 0;
						int cs, ls, cys, aux = 0;
						clear_simulation(); // reseta todas as variaveis de simulacao

						if (Context.device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(gui_id_console_window, true)) // pular 2 linhas
						{
							Context.listbox->insertItem(0, L"-----------------------------------------------------------------------------------", -1);
						}
						
						cs = _wtoi(cash_size_edit_box->getText());
						clock_ref_cycle = cs;
						ls = _wtoi(cash_line_size_edit_box->getText());

						if(!define_cache_size(cs))
						{
							if (Context.device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(gui_id_console_window, true))
								Context.listbox->insertItem(0, L"ERRO CS> O valor precisa ser multiplo de oito e o valor maximor é 2048 KB. Tente novamente.", -1);
						}
						else
						{
							stringw str = L"> Cache size define: ";
							str += cache_size;
							str += " bytes.";
							if (Context.device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(gui_id_console_window, true))
								Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);

							// podemos tentar definir a LC
							if(ls > 0)
								aux = define_cache_line_size(ls);

							if(aux == 0)
							{
								stringw str = L"ERRO LS> Valor não pode ser maior que 1024 bytes.";
								if (Context.device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(gui_id_console_window, true))
									Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
							}

							else if(aux > 1 || ls <= 0)
							{
								stringw str = L"ERRO LS> Valor da linha de cache nao atende os requisitos. O valor deve ser multiplo de dois e deve ser menor ou igual ";
								str += aux;
								str += " bytes. Tente novamente.";
								if (Context.device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(gui_id_console_window, true))
									Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
							}
							else
							{
								stringw str = L"> Cache line defined: ";
								str += cache_line_size;
								str += " bytes.";
								if (Context.device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(gui_id_console_window, true))
								{
									Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
									str = L"DMI: "; str += nbdmi; str += " lines created";
									Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
									str = L"DMD: "; str += nbdmd; str += " lines created";
									Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
									str = L"FA: "; str += nbfa; str += " lines created";
									Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
									str = L"SA-2W: "; str += nbsa/2; str += " lines created";
									Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
									str = L"SA-4W: "; str += nbsa/4; str += " lines created";
									Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
									str = L"SA-8W: "; str += nbsa/8; str += " lines created";
									Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
								}

								// define the other variables
								if(_wtoi(cycle_size_edit_box->getText()) >= 0)
								cys = _wtoi(cycle_size_edit_box->getText());
								clock_ref_cycle = cys;

								if(Context.device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(gui_id_console_window, true) && cys > 0)
								{
									stringw str = L"> Cycle defined: ";
									str += cys;
									str += " clock cycles.";
									Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
								}

								ffa_lru = (int)fa_lru->isChecked();
								fsa2w_lru = (int)sa2w_lru->isChecked();
								fsa4w_lru = (int)sa4w_lru->isChecked();
								fsa8w_lru = (int)sa8w_lru->isChecked();

								if (Context.device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(gui_id_console_window, true))
								{
									stringw str = L"> FA LRU: ";
									str += ffa_lru;
									Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
									str = L"> 2W LRU: ";
									str += fsa2w_lru;
									Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
									str = L"> 4W LRU: ";
									str += fsa4w_lru;
									Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
									str = L"> 8W LRU: ";
									str += fsa8w_lru;
									Context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
								}
								sok = 1;
							}
						}
						FILE *fpin = NULL;
						char filename[1024];
						wcstombs_s(NULL,filename,wcslen(eb_filename->getText())+1,eb_filename->getText(),wcslen(eb_filename->getText())+1);

						if(_wtoi(listboxrange->getText()) >= 2)
							add_range = _wtoi(listboxrange->getText());
						if(_wtoi(listboxnumberadd->getText()) >= 2)
							add_number = _wtoi(listboxnumberadd->getText());

						
						if(cb_openfromfile->isChecked()) // le arquivo arquivo
						{
							fpin = fopen(filename, "r");
							if(!fpin)
							{
								update_log(L"FILE> ERROR: Couldn't find the file.");
								fpin = NULL;
							}
							else
							{
								update_log(L"FILE> Reading file...");
								enderecos_ui = read_address_file(fpin);//le arquivo
							}
						}
						if(fpin == NULL) // gera endereços e um arquivo
							enderecos_ui = generate_add(add_number, add_range, cache_line_size); // gera endereços MUDAR DAQUI
						

						if(sok)
						{
							bus_end = enderecos_ui[end_index_ui].endereco;
							bus_end_type  = enderecos_ui[end_index_ui].type;
							update_informations(); // update the tables (cache watches)
						}
						if(fpin)
							fclose(fpin);

					}return true;

				case gui_id_open_settings_window:
					{
						IGUIEnvironment *env = Context.device->getGUIEnvironment();
						IGUIElement *root = env->getRootGUIElement();
						IGUIElement *e = root->getElementFromId(gui_id_settings_window, true);

						if (e)
						{
							//e->remove();
							return false;
						}

						/*
						IGUIElement* toolboxWnd = Context.device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(gui_id_settings_window, true);
						if (!toolboxWnd)
							return;
						*/

						IGUIWindow* window = env->addWindow(rect<s32>(0, 0 , 200, 300), false, L"Cache settings", 0, gui_id_settings_window);
						
						cash_size_edit_box = env->addEditBox(L"", rect<s32>(80, 40, 140, 60), true, window); env->addStaticText(L"CS (KB):", rect<s32>(20, 40, 80, 60), false, false, window);
						cash_line_size_edit_box = env->addEditBox(L"", rect<s32>(80, 60, 140, 80), true, window); env->addStaticText(L"CL (Bytes)", rect<s32>(20, 60, 80, 80), false, false, window);
						cycle_size_edit_box = env->addEditBox(L"", rect<s32>(80, 80, 140, 100), true, window); env->addStaticText(L"Clock cycle", rect<s32>(20, 80, 80, 100), false, false, window);
						fa_lru = env->addCheckBox(false, rect<s32>(80, 100, 100, 120), window); env->addStaticText(L"FA LRU", rect<s32>(20, 100, 80, 120), false, false, window);
						sa2w_lru = env->addCheckBox(false, rect<s32>(80, 120, 100, 140), window); env->addStaticText(L"2W LRU", rect<s32>(20, 120, 80, 140), false, false, window);
						sa4w_lru = env->addCheckBox(false, rect<s32>(80, 140, 100, 160), window); env->addStaticText(L"4W LRU", rect<s32>(20, 140, 80, 160), false, false, window);
						sa8w_lru = env->addCheckBox(false, rect<s32>(80, 160, 100, 180), window); env->addStaticText(L"8W LRU", rect<s32>(20, 160, 80, 180), false, false, window);
						listboxnumberadd = env->addEditBox(L"", rect<s32>(80, 180, 140, 200),true, window); env->addStaticText(L"Add number", rect<s32>(20, 180, 80, 200), false, false, window);
						listboxrange = env->addEditBox(L"", rect<s32>(80, 200, 140, 220),true, window); env->addStaticText(L"Add range", rect<s32>(20, 200, 80, 220), false, false, window);
						cb_openfromfile = env->addCheckBox(false, rect<s32>(80, 220, 100, 240), window); eb_filename = env->addEditBox(L"", rect<s32>(100, 220, 180, 240), true, window);  env->addStaticText(L"File", rect<s32>(20, 220, 80, 240), false, false, window);
						env->addButton(rect<s32>(20,240,80,280), window, apply_cache_varaibles, L"Apply", L"Apply cache configurations.");
					}
					return true;

				case gui_id_open_console_window:
					{
						IGUIEnvironment* env = Context.device->getGUIEnvironment();
						IGUIElement* root = env->getRootGUIElement();
						IGUIElement* e = root->getElementFromId(gui_id_console_window, true);

						if (e)
						{
							//e->remove();
							return false;
						}


						IGUIWindow* window_console = env->addWindow(rect<s32>(0, 400, 800, 600), false, L"Console", 0, gui_id_console_window);

						Context.listbox = env->addListBox(rect<s32>(0, 20, 799, 200), window_console, -1);
					}
					return true;

				case gui_id_open_general_window:
					{
						IGUIEnvironment* env = Context.device->getGUIEnvironment();
						IGUIElement* root = env->getRootGUIElement();
						IGUIElement* e = root->getElementFromId(gui_id_general_window, true);

						if (e)
						{
							//e->remove();
							return false;
						}


						IGUIWindow *general_information_window = env->addWindow(rect<s32>(0, 0, 449, 550), false, L"Simulation window", 0, gui_id_general_window);
						debugInfo = env->addStaticText (L"Debug", rect<s32>(1,20, 450, 490), true, true, general_information_window, -1, true);

						env->addButton(rect<s32>(10,490,60,510), general_information_window, gui_id_nextclock, L"+1 clcok", L"Simulate one clock.");
						env->addButton(rect<s32>(70,490,120,510), general_information_window, gui_id_nextcycle, L"+ cycle", L"Simulate the cycle clock.");
						env->addButton(rect<s32>(130,490,180,510), general_information_window, gui_id_executeall, L"Execute", L"Simulate until the end.");


						env->addButton(rect<s32>(10,520,70,540), general_information_window, gui_id_viewdmi, L"DMI Watch", L"Watch the Direct Mapped Instructions Cache lines.");
						env->addButton(rect<s32>(80,520,140,540), general_information_window, gui_id_viewdmd, L"DMD Watch", L"Watch the Direct Mapped Data Cache lines.");
						env->addButton(rect<s32>(150,520,210,540), general_information_window, gui_id_viewfa, L"FA Watch", L"Watch the Fully Associative Cache lines.");
						env->addButton(rect<s32>(220,520,280,540), general_information_window, gui_id_view2w, L"2W Watch", L"Watch the Set Associative two-way Cache lines.");
						env->addButton(rect<s32>(290,520,350,540), general_information_window, gui_id_view4w, L"4W Watch", L"Watch the Set Associative four-way Cache lines.");
						env->addButton(rect<s32>(360,520,420,540), general_information_window, gui_id_view8w, L"8W Watch", L"Watch the Set Associative eight-way Cache lines.");
					}
					return true;

				case gui_id_viewdmi:
					{
						int cache_line;
						IGUIEnvironment* env = Context.device->getGUIEnvironment();
						IGUIElement* root = env->getRootGUIElement();
						IGUIElement* e = root->getElementFromId(gui_id_viewdmiw, true);

						if (e)
						{
							//e->remove();
							return false;
						}

						IGUIWindow* watch_window = env->addWindow(rect<s32>(0, 0, 200, 200), false, L"DMI cache watch", 0, gui_id_viewdmiw);
						Context.listboxdmi = env->addListBox(rect<s32>(0, 20, 200, 200), watch_window, -1);
						//Context.listboxfa->insertItem(0, L"I     V     R     D     ADD", -1);
						update_informations();
					}
					return true;

				case gui_id_viewdmd:
					{
						int cache_line;
						IGUIEnvironment* env = Context.device->getGUIEnvironment();
						IGUIElement* root = env->getRootGUIElement();
						IGUIElement* e = root->getElementFromId(gui_id_viewdmdw, true);

						if (e)
						{
							//e->remove();
							return false;
						}

						IGUIWindow* watch_window = env->addWindow(rect<s32>(0, 0, 200, 200), false, L"DMD cache watch", 0, gui_id_viewdmdw);
						Context.listboxdmd = env->addListBox(rect<s32>(0, 20, 200, 200), watch_window, -1);
						//Context.listboxfa->insertItem(0, L"I     V     R     D     ADD", -1);
						update_informations();
					}
					return true;

				case gui_id_viewfa:
					{
						int cache_line;
						IGUIEnvironment* env = Context.device->getGUIEnvironment();
						IGUIElement* root = env->getRootGUIElement();
						IGUIElement* e = root->getElementFromId(gui_id_viewfaw, true);

						if (e)
						{
							//e->remove();
							return false;
						}

						IGUIWindow* watch_window = env->addWindow(rect<s32>(0, 0, 200, 200), false, L"FA cache watch", 0, gui_id_viewfaw);
						Context.listboxfa = env->addListBox(rect<s32>(0, 20, 200, 200), watch_window, -1);
						//Context.listboxfa->insertItem(0, L"I     V     R     D     ADD", -1);
						update_informations();
					}
					return true;

				case gui_id_view2w:
					{
						int cache_line, cache_col;
						IGUIEnvironment* env = Context.device->getGUIEnvironment();
						IGUIElement* root = env->getRootGUIElement();
						IGUIElement* e = root->getElementFromId(gui_id_view2ww, true);

						if (e)
						{
							//e->remove();
							return false;
						}

						IGUIWindow* watch_window = env->addWindow(rect<s32>(0, 0, 300, 200), false, L"SA2W cache watch", 0, gui_id_view2ww);
						Context.listbox2w = env->addListBox(rect<s32>(0, 20, 300, 200), watch_window, -1);
						update_informations();
					}
					return true;

					case gui_id_view4w:
					{
						int cache_line, cache_col;
						IGUIEnvironment* env = Context.device->getGUIEnvironment();
						IGUIElement* root = env->getRootGUIElement();
						IGUIElement* e = root->getElementFromId(gui_id_view4ww, true);

						if (e)
						{
							//e->remove();
							return false;
						}

						IGUIWindow* watch_window = env->addWindow(rect<s32>(0, 0, 600, 200), false, L"SA4W cache watch", 0, gui_id_view4ww);
						Context.listbox4w = env->addListBox(rect<s32>(0, 20, 600, 200), watch_window, -1);
						update_informations();
					}
					return true;

					case gui_id_view8w:
					{
						int cache_line, cache_col;
						IGUIEnvironment* env = Context.device->getGUIEnvironment();
						IGUIElement* root = env->getRootGUIElement();
						IGUIElement* e = root->getElementFromId(gui_id_view8ww, true);

						if (e)
						{
							//e->remove();
							return false;
						}

						IGUIWindow* watch_window = env->addWindow(rect<s32>(0, 0, 1000, 200), false, L"SA8W cache watch", 0, gui_id_view8ww);
						Context.listbox8w = env->addListBox(rect<s32>(0, 20, 1000, 200), watch_window, -1);
						update_informations();
					}
					return true;

					case gui_id_nextclock:
					{
						if(end_index_ui < add_number && sok && bus_end > 0) // terminou simulação?
						{
							cache_simulation_ui();
							update_informations();
						}
					}
					return true;

					case gui_id_nextcycle:
					{
						if(end_index_ui < add_number && sok && bus_end > 0)
						{
							int aux;
							for(aux = 0; aux < clock_ref_cycle; aux++)
								if(end_index_ui < add_number)
								{
									cache_simulation_ui();
									
								}
							update_informations();
						}
					}
					return true;

					case gui_id_executeall:
					{
						if(end_index_ui < add_number && sok && bus_end > 0)
						{
							int aux;
							do
							{
								cache_simulation_ui();
							}while(end_index_ui <= add_number && sok);
						}
						update_informations();
					}
					return true;
					

				default:
					return false;
				}
				break;
			default:
				break;
			}
		}
		return false;
	}

private:
	SAppContext & Context;
};

SAppContext context; // IMPORTANT!!!

void update_log(stringw str)
{
	if(context.device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(gui_id_console_window, true))
		context.listbox->insertItem(0, stringw(str.c_str()).c_str(), -1);
}


void update_informations()
{
	int cache_line, cache_col;
	int aux; // to clear lsitboxes

	IGUIEnvironment* env = context.device->getGUIEnvironment();
	IGUIElement* root = env->getRootGUIElement();
	IGUIElement* e;

	// DMI
	e = root->getElementFromId(gui_id_viewdmiw, true);
	if (e)
	{
		for (aux = 0; aux < nbdmi; aux++)
			context.listboxdmi->removeItem(0);

		for(cache_line = 0; cache_line < nbdmi; cache_line++)
		{
			stringw str = L"";
			str += cache_line; str += ">  V:";
			str += DM_I[cache_line].bv; str += "  A:";
			str += DM_I[cache_line].tag;
			context.listboxdmi->insertItem(cache_line,stringw(str.c_str()).c_str(), -1);
		}
	}

	// DMD
	e = root->getElementFromId(gui_id_viewdmdw, true);
	if (e)
	{
		for (aux = 0; aux < nbdmd; aux++)
			context.listboxdmd->removeItem(0);

		for(cache_line = 0; cache_line < nbdmd; cache_line++)
		{
			stringw str = L"";
			str += cache_line; str += ">  V:";
			str += DM_D[cache_line].bv; str += "  D:";
			str += DM_D[cache_line].d;str += "  A:";
			str += DM_D[cache_line].tag;
			context.listboxdmd->insertItem(cache_line,stringw(str.c_str()).c_str(), -1);
		}
	}

	// FA
	e = root->getElementFromId(gui_id_viewfaw, true);
	if (e)
	{
		for (aux = 0; aux < nbfa; aux++)
			context.listboxfa->removeItem(0);

		for(cache_line = 0; cache_line < nbfa; cache_line++)
		{
			irr::core::stringw nature = "?";
			if(FA[cache_line].nature == 100)
				nature = "D";
			else if(FA[cache_line].nature == 105)
				nature = "I";

			stringw str = L"";
			str += cache_line; str += ">  V:";
			str += FA[cache_line].bv; str += "  R:";
			str += FA[cache_line].r; str += "  D:";
			str += FA[cache_line].d; str += "  A:";
			str += FA[cache_line].tag; str += nature;	
			context.listboxfa->insertItem(cache_line,stringw(str.c_str()).c_str(), -1);
		}
	}

	// 2w
	e = root->getElementFromId(gui_id_view2ww, true);
	if (e)
	{
		for (aux = 0; aux < (nbsa/2); aux++)
			context.listbox2w->removeItem(0);

		for(cache_line = 0; cache_line < (nbsa/2); cache_line++)
		{
			stringw str = L""; str += cache_line; str += ">  ";
			for(cache_col = 0; cache_col< 2; cache_col++)
			{
				irr::core::stringw nature = "?";
				if(SA2W[cache_line][cache_col].nature == 100)
					nature = "D";
				else if(SA2W[cache_line][cache_col].nature == 105)
					nature = "I";

				str += "V:";
				str += SA2W[cache_line][cache_col].bv; str += " R:";
				str += SA2W[cache_line][cache_col].r; str += " D:";
				str += SA2W[cache_line][cache_col].d; str += " A:";
				str += SA2W[cache_line][cache_col].tag; str += nature;
				str += "     ";
			}
			context.listbox2w->insertItem(cache_line,stringw(str.c_str()).c_str(), -1);
		}
	}

	// 4w
	e = root->getElementFromId(gui_id_view4ww, true);
	if (e)
	{
		for (aux = 0; aux < (nbsa/4); aux++)
			context.listbox4w->removeItem(0);

		for(cache_line = 0; cache_line < (nbsa/4); cache_line++)
		{
			stringw str = L""; str += cache_line; str += ">  ";
			for(cache_col = 0; cache_col< 4; cache_col++)
			{
				irr::core::stringw nature = "?";
				if(SA4W[cache_line][cache_col].nature == 100)
					nature = "D";
				else if(SA4W[cache_line][cache_col].nature == 105)
					nature = "I";

				str += "V:";
				str += SA4W[cache_line][cache_col].bv; str += " R:";
				str += SA4W[cache_line][cache_col].r; str += " D:";
				str += SA4W[cache_line][cache_col].d; str += " A:";
				str += SA4W[cache_line][cache_col].tag; str += nature;
				str += "     ";
			}
			context.listbox4w->insertItem(cache_line,stringw(str.c_str()).c_str(), -1);
		}
	}

	// 8w
	e = root->getElementFromId(gui_id_view8ww, true);
	if (e)
	{
		for (aux = 0; aux < (nbsa/8); aux++)
			context.listbox8w->removeItem(0);

		for(cache_line = 0; cache_line < (nbsa/8); cache_line++)
		{
			stringw str = L""; str += cache_line; str += ">  ";
			for(cache_col = 0; cache_col< 8; cache_col++)
			{
				irr::core::stringw nature = "?";
				if(SA8W[cache_line][cache_col].nature == 100)
					nature = "D";
				else if(SA8W[cache_line][cache_col].nature == 105)
					nature = "I";

				str += "V:";
				str += SA8W[cache_line][cache_col].bv; str += " R:";
				str += SA8W[cache_line][cache_col].r; str += " D:";
				str += SA8W[cache_line][cache_col].d; str += " A:";
				str += SA8W[cache_line][cache_col].tag; str += nature;
				str += "     ";
			}
			context.listbox8w->insertItem(cache_line,stringw(str.c_str()).c_str(), -1);
		}
	}

	e = root->getElementFromId(gui_id_general_window, true);
	if (e)
	{
		stringw str = L"";
		str += "GENERAL CLOCK: "; str += gen_clock; str += "\n\n";

		str += "Addres index: "; str += end_index_ui; str += " <";str += "Addres: "; str += bus_end; str += ", "; str += bus_end_type;str += "\n\n";

		str += "--DMI--\n"; str += "hit:"; str += dmi_hit; str += "\n"; str += "miss:";str += dmi_miss; str += "\n"; str += "HIT/MISS RATIO:   "; if(dmi_miss != 0)str += dmi_hit/dmi_miss; str += "\n\n";
		str += "--DMD--\n"; str += "hit:"; str += dmd_hit; str += "\n"; str += "miss:";str += dmd_miss; str += "\n"; str += "HIT/MISS RATIO:   "; if(dmd_miss != 0)str += dmd_hit/dmd_miss; str += "\n\n";
		str += "--FA--\n"; str += "hit:"; str += fa_hit; str += "\n"; str += "miss:";str += fa_miss; str += "\n"; str += "HIT/MISS RATIO:   "; if(fa_miss != 0)str += fa_hit/fa_miss; str += "\n\n";

		str += "--2W--\n"; str += "hit:"; str += sa2w_hit; str += "\n"; str += "miss:";str += sa2w_miss; str += "\n"; str += "HIT/MISS RATIO:   "; if(sa2w_miss != 0)str += sa2w_hit/sa2w_miss; str += "\n\n";

		str += "--4W--\n"; str += "hit:"; str += sa4w_hit; str += "\n"; str += "miss:";str += sa4w_miss; str += "\n"; str += "HIT/MISS RATIO:   "; if(sa4w_miss != 0)str += sa4w_hit/sa4w_miss; str += "\n\n";

		str += "--8W--\n"; str += "hit:"; str += sa8w_hit; str += "\n"; str += "miss:";str += sa8w_miss; str += "\n"; str += "HIT/MISS RATIO:   "; if(sa8w_miss != 0)str += sa8w_hit/sa8w_miss; str += "\n\n";

		e = root->getElementFromId(gui_id_general_window, true);
		if (e)
		{
			debugInfo->setText(str.c_str());
		}
	}
}


int main()
{
	// device
	IrrlichtDevice * device = createDevice(EDT_OPENGL, core::dimension2d<u32>(1000, 600), 16U, false);
	if (device == 0)
	{
		printf("Erro ao inicializar a interface grafica."); fflush(stdin); getchar();
		return 1;
	}
	device->setWindowCaption(L"Simulador de Memória Cache - DEVELOPER RELEASE (programmer - Armais Hornung - aramishornungmoraes@gmail.com)");
	device->setResizable(true);
	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();

	// UI
	IGUISkin* skin = env->getSkin();
	IGUIFont* font = env->getFont("fonthaettenschweiler.bmp");
	if (font)
		skin->setFont(font);
	//skin->setFont(env->getBuiltInFont(), EGDF_TOOLTIP);

	for (u32 i=0; i<EGDC_COUNT ; ++i)
	{
		SColor col = env->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
		col.setAlpha(255);
		env->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
	}env->getSkin()->setColor((EGUI_DEFAULT_COLOR)2, SColor(255, 63, 63, 31));env->getSkin()->setColor((EGUI_DEFAULT_COLOR)1, SColor(255, 63, 63, 31));env->getSkin()->setColor((EGUI_DEFAULT_COLOR)3, SColor(255, 127, 127, 63));env->getSkin()->setColor((EGUI_DEFAULT_COLOR)8, SColor(255, 255, 255, 127));env->getSkin()->setColor((EGUI_DEFAULT_COLOR)10, SColor(255, 31, 31, 15));env->getSkin()->setColor((EGUI_DEFAULT_COLOR)12, SColor(255, 31, 31, 15));env->getSkin()->setColor((EGUI_DEFAULT_COLOR)13, SColor(255, 255, 0, 0)); env->getSkin()->setColor((EGUI_DEFAULT_COLOR)16, SColor(255, 63, 255, 63)); env->getSkin()->setColor((EGUI_DEFAULT_COLOR)22, SColor(255, 127, 127, 63));env->getSkin()->setColor((EGUI_DEFAULT_COLOR)24, SColor(255, 63, 127, 63));env->getSkin()->setColor((EGUI_DEFAULT_COLOR)25, SColor(255, 63, 255, 63));
	//env->addImage(driver->getTexture("irrlichtlogoalpha2.tga"), position2d<int>(10,10));


	// Design
	// BUTTONS
	env->addButton(rect<s32>(200,0,300,30), 0, GUI_ID_QUIT_BUTTON, L"Quit", L"Exits Program.");
	env->addButton(rect<s32>(305,0,405,30), 0, gui_id_open_settings_window, L"Cache Settings", L"Define the cache simulator's variables.");
	env->addButton(rect<s32>(410,0,510,30), 0, gui_id_open_console_window, L"Console", L"Show up the console.");
	env->addButton(rect<s32>(515,0,615,30), 0, gui_id_open_general_window, L"Simulation", L"Opens a window with informations and the controls for simulation.");

	// Store the appropriate data in a context structure.
	context.device = device;
	context.counter = 0;

	// Event.OP
	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);

	//loop run

	while(device->run() && driver)
	{
		driver->beginScene(true, true, SColor(0,127,127,63));
		env->drawAll();
		driver->endScene();
	}

	device->drop();

	return 0;
}