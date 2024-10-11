#include "spriteArea.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

extern Glib::RefPtr<Gtk::Application>   app;

spriteArea::spriteArea():Glib::ObjectBase("mySpriteArea"),Gtk::Widget(),
    m_cellSize(64),m_nbCells(8),m_top(0),m_bottom(0),m_left(0), m_right(0),
    m_sprite_width(32),m_sprite_height(32),m_id_select(0),m_i_sequence(-1),
	m_move_up_item(NULL),m_move_down_item(NULL),
	m_delete_item(NULL),m_Menu_Popup(NULL)
{
    //ctor
    set_has_window(true);

	try
	{
		m_liste_sprites[0] = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, TRUE, 8, 32, 32);
		m_liste_sprites[0]->fill(0x00000000);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	

	for (int i=0;i<m_nbCells;i++){
		m_liste_modif_flags[i] = false;
	}

	//Fill popup menu:
	//m_move_up_item = Gtk::make_managed<Gtk::MenuItem>("Move Up", true);
	m_move_up_item = new Gtk::MenuItem("Move Up", true);
	m_move_up_item->signal_activate().connect(sigc::mem_fun(*this, &spriteArea::on_menu_popup_move_up) );

	//m_move_down_item = Gtk::make_managed<Gtk::MenuItem>("Move Down", true);
	m_move_down_item = new Gtk::MenuItem("Move Down", true);
	m_move_down_item->signal_activate().connect(sigc::mem_fun(*this, &spriteArea::on_menu_popup_move_down) );
 
	m_separator_item = new Gtk::SeparatorMenuItem();
 
	m_delete_item = new Gtk::MenuItem("Delete", true);
	m_delete_item->signal_activate().connect(sigc::mem_fun(*this, &spriteArea::on_menu_popup_delete) );
 

}

spriteArea::~spriteArea()
{
    //dtor
	//PauseSequence();
	if (m_move_up_item){
		delete m_move_up_item;
	}
	if (m_move_down_item){
		delete m_move_down_item;
	}
	if (m_separator_item){
		delete m_separator_item;
	}
	if (m_delete_item){
		delete m_delete_item;
	}
    
}

void spriteArea::get_preferred_width_vfunc(int &minimum_width, int &natural_width) const
{
	minimum_width = 64;
	natural_width = 64;
}

void spriteArea::get_preferred_height_for_width_vfunc(int /* width */,
													  int &minimum_height, int &natural_height) const
{
	minimum_height = 64 * 9;
	natural_height = 64 * 9;
}

void spriteArea::get_preferred_height_vfunc(int &minimum_height, int &natural_height) const
{
	minimum_height = 64*9;
	natural_height = 64*9;
}

void spriteArea::get_preferred_width_for_height_vfunc(int /* height */,
													  int &minimum_width, int &natural_width) const
{
	minimum_width = 64;
	natural_width = 64;
}

void spriteArea::on_size_allocate(Gtk::Allocation &allocation)
{
	//Do something with the space that we have actually been given:
	//(We will not be given heights or widths less than we have requested, though
	//we might get more)

	//Use the offered allocation for this container:
	set_allocation(allocation);

	if (m_refGdkWindow)
	{
		m_refGdkWindow->move_resize(allocation.get_x(), allocation.get_y(),
									allocation.get_width(), allocation.get_height());
	}
}

void spriteArea::on_map()
{
    // Call base class:
    Gtk::Widget::on_map();
}

void spriteArea::on_unmap()
{
    // Call base class:
    Gtk::Widget::on_unmap();
}

void spriteArea::on_realize()
{
    // Call base class:

    set_realized();

    if (!m_refGdkWindow){

        //--Create the GdkWindow
        GdkWindowAttr attributes;
        memset(&attributes,0,sizeof(attributes));

        Gtk::Allocation allocation = get_allocation();

        //-- Set initial position and size of the Gdk::Window
        attributes.x = allocation.get_x();
        attributes.y = allocation.get_y();
        attributes.width = allocation.get_width();
        attributes.height = allocation.get_height();

        attributes.event_mask =get_events() | Gdk::EXPOSURE_MASK | Gdk::BUTTON1_MASK | Gdk::BUTTON3_MASK
                                            | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_RELEASE_MASK
                                            | Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK
                                            | Gdk::POINTER_MOTION_HINT_MASK;
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.wclass = GDK_INPUT_OUTPUT;

        m_refGdkWindow = Gdk::Window::create(get_parent_window(),&attributes,GDK_WA_X|GDK_WA_Y);

        set_window(m_refGdkWindow);

        //--Set colors
        //modify_bg(Gtk::STATE_NORMAL, Gdk::Color("white"));
        //modify_fg(Gtk::STATE_NORMAL, Gdk::Color("blue"));

        // make the widget recieve exposure events
        m_refGdkWindow->set_user_data(gobj());

    }

}

void spriteArea::on_unrealize()
{
    m_refGdkWindow.clear();

    //Call base class:
    Gtk::Widget::on_unrealize();
}

int spriteArea::CoordYtoIndex(int y)
{
	//------------------------------------------------------------
	if ((y > m_top) && (y < m_bottom))
	{
		return y / m_cellSize;
	}
	return -1;
}

bool spriteArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
	const int width = get_allocation().get_width();
	const int height = get_allocation().get_height();

	// paint the background
	Gdk::Cairo::set_source_rgba(cr, get_style_context()->get_background_color());
	cr->paint();

	//-- Draw Frame
	int x = 0;
	int y = 0;
	m_top = y;
	cr->set_source_rgba(0.5, 0.5, 0.5, 1.0);
	for (int i = 0; i < m_nbCells; i++)
	{
		cr->move_to(0, y);
		cr->line_to(m_cellSize, y);
		y += m_cellSize;
	}
	cr->move_to(0, y);
	cr->line_to(m_cellSize, y);
	m_bottom = y;
	//--
	cr->move_to(0, m_top);
	cr->line_to(0, m_bottom);
	cr->move_to(m_cellSize - 1, m_top);
	cr->line_to(m_cellSize - 1, m_bottom);
	cr->stroke();

	//-- Draw Sprites
	m_left = 0;
	m_right = m_cellSize;
	for (int i = 0; i < m_nbCells; i++)
	{
		if (auto sprite=m_liste_sprites[i])
		{
			auto w = sprite->get_width();
			auto h = sprite->get_height();
			x = (m_left + m_right) / 2 - (w / 2);
			y = i * m_cellSize + m_cellSize / 2 - (h / 2);
			Gdk::Cairo::set_source_pixbuf(cr, m_liste_sprites[i], x, y);
			cr->paint();
		}
	}

	//--
	if (m_i_sequence>=0){
		if ( auto sprite=m_liste_sprites[m_i_sequence]){
			int left = 0;
			int right = m_cellSize;
			auto w = sprite->get_width();
			auto h = sprite->get_height();
			x = (left + right) / 2 - (w / 2);
			y = m_nbCells * m_cellSize + m_cellSize / 2 - (h / 2);
			Gdk::Cairo::set_source_pixbuf(cr, sprite, x, y);
			cr->paint();
			//-- Draw sequence mark
			int top = m_i_sequence * m_cellSize;
			int bottom = top + m_cellSize;
			left += 3;
			top += 3;
			right -= 3;
			bottom -= 3;
			cr->set_source_rgba(0.0, 0.0, 0.9, 1.0);
			cr->move_to(left, top);
			cr->line_to(right, top);
			cr->line_to(right, bottom);
			cr->line_to(left, bottom);
			cr->line_to(left, top);
			cr->stroke();

		}
	}

	// Draw Select mark
	if ((m_id_select >= 0) && (m_id_select<m_nbCells))
	{
		int left = m_left;
		int right = m_right;
		int top = m_id_select * m_cellSize;
		int bottom = top + m_cellSize;

		left += 2;
		top += 2;
		right -= 2;
		bottom -= 2;
		cr->set_source_rgba(0.9, 0.0, 0.0, 1.0);
		//-- Haut Gauche
		cr->move_to(left, top + 8);
		cr->line_to(left, top);
		cr->line_to(left + 8, top);
		//cr->stroke();
		//-- Haut Droite
		cr->move_to(right, top + 8);
		cr->line_to(right, top);
		cr->line_to(right - 8, top);
		//cr->stroke();
		//-- Bas Droite
		cr->move_to(right, bottom - 8);
		cr->line_to(right, bottom);
		cr->line_to(right - 8, bottom);
		//cr->stroke();
		//-- Bas Gauche
		cr->move_to(left, bottom - 8);
		cr->line_to(left, bottom);
		cr->line_to(left + 8, bottom);
		cr->stroke();

	}

	return true;
}

bool spriteArea::on_button_press_event(GdkEventButton *event)
{
	int x, y;
	int id,id_bak;
	//Gdk::ModifierType   state= Gdk::ModifierType(0);
	//----------------------------------------------------------
	//m_refGdkWindow->get_pointer(x,y,state);

	id = CoordYtoIndex(event->y);

	if (event->button == 1)
	{
		if ((id >= 0)&&(id<m_nbCells))
		{
			if (!m_liste_sprites[id])
			{
				id_bak = m_id_select;
				m_id_select = id;
				if (!CreateNewSprite()){
					m_id_select = id_bak;
				}
			}else{
				m_id_select = id;
			}
			m_signal_sprite_pick.emit(m_liste_sprites[id]);
		}

	}else if (event->button == 3){
 		if ((id >= 0)&&(id<m_nbCells))
		{
			m_y_right_click = event->y;
			if (m_Menu_Popup){
				delete m_Menu_Popup;
			}
			m_Menu_Popup = new Gtk::Menu();
			if (id==0){
				m_Menu_Popup->append(*m_move_down_item);
			}else if (id==(m_nbCells-1)){
				m_Menu_Popup->append(*m_move_up_item);
			}else{
				m_Menu_Popup->append(*m_move_up_item);
				m_Menu_Popup->append(*m_move_down_item);
			}
			if (m_liste_sprites[id]){
				m_Menu_Popup->append(*m_separator_item);
				m_Menu_Popup->append(*m_delete_item);
			}

			m_Menu_Popup->show_all();
  			m_Menu_Popup->popup_at_pointer((GdkEvent*) event);

		}
	}
	m_refGdkWindow->invalidate(true);

	return true;
}

void spriteArea::on_sprite_modified()
{
	//--------------------------------------------------------
	m_liste_modif_flags[m_id_select] = true;
	m_refGdkWindow->invalidate(true);
}

void spriteArea::SetSprite(Glib::RefPtr<Gdk::Pixbuf> newSprite)
{
	//--------------------------------------------------------
	if ((m_id_select>=0)&&(m_id_select<m_nbCells)){
		if (!newSprite->get_has_alpha()){
			m_liste_sprites[m_id_select] = newSprite->add_alpha(true,0,0,0); // RGBA
		}else{
			m_liste_sprites[m_id_select] = newSprite;
		}
        m_sprite_width  = newSprite->get_width();
        m_sprite_height = newSprite->get_height();
		m_refGdkWindow->invalidate(true);
		m_signal_sprite_pick.emit(m_liste_sprites[m_id_select]);

	}

}

Glib::RefPtr<Gdk::Pixbuf> spriteArea::GetSprite()
{
	//--------------------------------------------------------
	return m_liste_sprites[m_id_select];

}

Glib::ustring spriteArea::GetSpriteName()
{
	return m_liste_names[m_id_select];
}

void spriteArea::SetSpriteName(Glib::ustring name)
{
	m_liste_names[m_id_select] = name;
}

spriteArea::type_signal_sprite_pick spriteArea::signal_sprite_pick()
{
    //------------------------------------------------
    return m_signal_sprite_pick;

}

bool spriteArea::CreateNewSprite()
{
   	int widthSprite, heightSprite;
    //------------------------------------------------------------
    Gtk::Dialog dlg("New Sprite");
    dlg.set_transient_for(*(app->get_active_window()));
    dlg.set_default_size( 320, 128);
  
    Gtk::Button *button;
    button = new Gtk::Button(Gtk::Stock::OK);
    button->show(); 
    dlg.add_action_widget(*button,Gtk::RESPONSE_OK); 		
    button = new Gtk::Button(Gtk::Stock::CANCEL);
    button->show(); 
    dlg.add_action_widget(*button,Gtk::RESPONSE_CANCEL);

    Gtk::Label *label;
    Gtk::VBox   vbox1;

    Gtk::HBox   hbox1;
    label = new Gtk::Label("Width ");
    Gtk::Entry *widthEntry = new Gtk::Entry();
    widthEntry->set_text("32");
    widthEntry->set_width_chars(5);
    hbox1.pack_start(*label,Gtk::PACK_EXPAND_WIDGET,16);    
    hbox1.pack_end(*widthEntry,Gtk::PACK_SHRINK,16);
    vbox1.pack_start(hbox1,Gtk::PACK_SHRINK);

    Gtk::HBox   hbox2;
    label = new Gtk::Label("Height ");
    Gtk::Entry *heightEntry = new Gtk::Entry();
    heightEntry->set_text("32");
    heightEntry->set_width_chars(5);
    hbox2.pack_start(*label,Gtk::PACK_EXPAND_WIDGET,16);    
    hbox2.pack_end(*heightEntry,Gtk::PACK_SHRINK,16);
    vbox1.pack_start(hbox2,Gtk::PACK_SHRINK,10);

    vbox1.show_all_children();
    vbox1.show_all();
    dlg.get_content_area()->pack_start(vbox1,Gtk::PACK_SHRINK,8);

    int result = dlg.run();
    switch(result){
    case Gtk::RESPONSE_OK:
        {
            std::cout << "RESPONSE OK" << std::endl;
            widthSprite = atoi(widthEntry->get_text().c_str());
            heightSprite = atoi(heightEntry->get_text().c_str());
            SetSpriteName("");
            Glib::RefPtr<Gdk::Pixbuf> sprite = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, TRUE, 8, widthSprite, heightSprite);
            sprite->fill(0x00000000);
            SetSprite(sprite);
			//-- Markage pour faire la sauvegarde
			m_liste_modif_flags[m_id_select] = true;
			return true;
        }
        break;
    case Gtk::RESPONSE_CANCEL:
		{
			std::cout << "RESPONSE CANCEL" << std::endl;
		}
        break;
    }

	return false;

}

bool spriteArea::on_timeout(int timer_number)
{
    //------------------------------------------------------------
    std::cout << "..." << std::endl;
	do{
		m_i_sequence++;
		if (m_i_sequence>=m_nbCells){
			m_i_sequence = 0;
		}
	}while(!m_liste_sprites[m_i_sequence]); //-- Sauter les cellules sans image
	m_refGdkWindow->invalidate(true);

	return true;

}

void spriteArea::PlaySequence()
{
    //------------------------------------------------------------
    std::cout << "Play..." << std::endl;

	//-- Define slot
	sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this,&spriteArea::on_timeout), 1);

	// This is where we connect the slot to the Glib::signal_timeout()
	m_connection_timer = Glib::signal_timeout().connect(my_slot,950);


}

void spriteArea::PauseSequence()
{
    //------------------------------------------------------------
    std::cout << "Pause..." << std::endl;
    // Diconnect the signal handler:
    m_connection_timer.disconnect();
	m_i_sequence = -1;
	m_refGdkWindow->invalidate(true);

}

void spriteArea::on_menu_popup_move_up()
{
	Glib::RefPtr<Gdk::Pixbuf>	tmpSprite;
	std::string					fileName;
	bool						modifFlag;
    //------------------------------------------------------------
    std::cout << "Move Up" << std::endl;
	int id = CoordYtoIndex(m_y_right_click);
	if ((id>0)&&(id<m_nbCells)){
		int	idPrec = id - 1;
		//--
		tmpSprite = m_liste_sprites[idPrec];
		m_liste_sprites[idPrec] = m_liste_sprites[id];
		m_liste_sprites[id] = tmpSprite;
		//--
		fileName = m_liste_names[idPrec];
		m_liste_names[idPrec] = m_liste_names[id];
		m_liste_names[id] = fileName;
		//--
		modifFlag = m_liste_modif_flags[idPrec];
		m_liste_modif_flags[idPrec] = m_liste_modif_flags[id];
		m_liste_modif_flags[id] = modifFlag;

		if (id==m_id_select){
			m_id_select = idPrec;
		}else if (idPrec==m_id_select){
			m_id_select=id;
		}
		m_refGdkWindow->invalidate(true);

	}
}

void spriteArea::on_menu_popup_move_down()
{
	Glib::RefPtr<Gdk::Pixbuf>	tmpSprite;
	std::string					fileName;
	bool						modifFlag;
    //------------------------------------------------------------
    std::cout << "Move Down" << std::endl;
	int id = CoordYtoIndex(m_y_right_click);
	if ((id>=0)&&(id<(m_nbCells-1))){
		int	idSuiv = id + 1;
		//--
		tmpSprite = m_liste_sprites[idSuiv];
		m_liste_sprites[idSuiv] = m_liste_sprites[id];
		m_liste_sprites[id] = tmpSprite;
		//--
		fileName = m_liste_names[idSuiv];
		m_liste_names[idSuiv] = m_liste_names[id];
		m_liste_names[id] = fileName;
		//--
		modifFlag = m_liste_modif_flags[idSuiv];
		m_liste_modif_flags[idSuiv] = m_liste_modif_flags[id];
		m_liste_modif_flags[id] = modifFlag;

		if (id==m_id_select){
			m_id_select = idSuiv;
		}else if (idSuiv==m_id_select){
			m_id_select = id;
		}
		m_refGdkWindow->invalidate(true);
	}
}

void spriteArea::SaveSpritesProj(std::string pathName,std::string fileName)
{
	std::string		spriteName;
	std::string		strName;
	std::string		noSuffixeProjectName;
	std::string		fullPathName;
    //------------------------------------------------------------
	fullPathName = pathName + "/" + fileName;
    std::ofstream   f(fullPathName);
    if (f.is_open()){
		//-- Remove suffixe
		size_t p = fileName.find_last_of(".proj");
		if (std::string::npos != p){
			noSuffixeProjectName = fileName.substr(0,fileName.size()-5);
		}
		//--
		for(int i=0;i<m_nbCells;i++){
			if (m_liste_sprites[i]){
				strName = m_liste_names[i];
				if (strName==""){
					spriteName = noSuffixeProjectName + "_" + std::to_string(i) + ".png";
				}else{
  					std::size_t found = strName.find_last_of("/\\");
					spriteName = strName.substr(found+1);
				}
				if (m_liste_modif_flags[i]){
					fullPathName = pathName + "/" + spriteName;
            		m_liste_sprites[i]->save(fullPathName, "png");
				}
			}else{
				spriteName = "NULL";
			}
			//--
			f << "SPRITE " << spriteName << std::endl;

		}
		//--
        f.close();
    }

}

void spriteArea::LoadSpritesProj(std::string fullPathName)
{
	int				i,id_sel_sprite;
    std::ifstream   f(fullPathName);
    //------------------------------------------------------------
    if (f.is_open()){
		std::string pathName;
		//-- Effacer la liste
		for( i=0; i<m_nbCells; i++){
			m_liste_sprites[i].reset();
			m_liste_names[i] = "";
			m_liste_modif_flags[i] = false;
		}
		//--
  		std::size_t found = fullPathName.find_last_of("/\\");
		if (found==std::string::npos){
			pathName = "";
		}else{
  			pathName = fullPathName.substr(0,found);
		}	
		//--
		i = 0;
		id_sel_sprite = -1;
		std::string strWord;
		std::string strline;
        while(!f.eof()){
            std::getline(f,strline);
            std::stringstream ss(strline);
            while (ss >> strWord){
                if (strWord=="SPRITE"){
					//--
					ss >> strWord;
					if (strWord!="NULL"){
						fullPathName = pathName + "/" + strWord;
            			Glib::RefPtr<Gdk::Pixbuf> newSprite = Gdk::Pixbuf::create_from_file(fullPathName);
						m_liste_names[i] = fullPathName;
						if (!newSprite->get_has_alpha()){
							m_liste_sprites[i] = newSprite->add_alpha(true,0,0,0);;
						}else{
							m_liste_sprites[i] = newSprite;
						}
						if (id_sel_sprite==-1){
							id_sel_sprite = i;
						}
					}
					//--
					i++;
                }
            }
        }

		//--
        f.close();

		//--
		if (id_sel_sprite==-1){
			m_id_select = 0;
		}else{
			m_id_select = id_sel_sprite;
		}
		if (m_liste_sprites[m_id_select]){
			Glib::RefPtr<Gdk::Pixbuf> sprite = m_liste_sprites[m_id_select];
			m_signal_sprite_pick.emit(sprite);
			m_sprite_width  = sprite->get_width();
			m_sprite_height = sprite->get_height();
		}
		m_refGdkWindow->invalidate(true);
    }

}

void spriteArea::InitSprites()
{
    //------------------------------------------------------------
	Glib::RefPtr<Gdk::Pixbuf>   nullPixbuf(NULL);
	//-- Effacer la liste
	for(int i=0; i<m_nbCells; i++){
		m_liste_sprites[i].reset();
		m_liste_modif_flags[i] = false;
		m_liste_names[i] = "";
	}
	m_id_select = 0;
	try
	{
		m_liste_sprites[0] = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, TRUE, 8, 32, 32);
		m_liste_sprites[0]->fill(0x00000000);
		m_sprite_width  = m_liste_sprites[0]->get_width();
		m_sprite_height = m_liste_sprites[0]->get_height();
		m_signal_sprite_pick.emit(m_liste_sprites[0]);
		m_refGdkWindow->invalidate(true);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

}

#define ID_REMOVE 1001
#define ID_DELETE 1002
void spriteArea::on_menu_popup_delete()
{
	std::string		msg;
	std::string		fileName;
	//---------------------------------------------------------
	int id = CoordYtoIndex(m_y_right_click);
	if ((id>=0)&&(id<(m_nbCells-1))){

		Gtk::Dialog dlg("Delete Sprite");
		dlg.set_transient_for(*(app->get_active_window()));
		dlg.set_default_size( 256, 128);
	
		Gtk::Button *button;
		button = new Gtk::Button("Remove");
		button->show(); 
		dlg.add_action_widget(*button,ID_REMOVE);
		button = new Gtk::Button("Delete");
		button->show(); 
		dlg.add_action_widget(*button,ID_DELETE);
		button = new Gtk::Button(Gtk::Stock::CANCEL);
		button->show(); 
		dlg.add_action_widget(*button,Gtk::RESPONSE_CANCEL);


		Gtk::VBox   vbox1;
		Gtk::Label	*label;
		fileName = Glib::path_get_basename(m_liste_names[id]);
		msg = "Choose Remove to remove : " + fileName;
		label = new Gtk::Label(msg);
		vbox1.pack_start(*label,Gtk::PACK_SHRINK);

		msg = "Choose Delete to delete : " + fileName;
		label = new Gtk::Label(msg);
		vbox1.pack_start(*label,Gtk::PACK_SHRINK);

		vbox1.show_all_children();
		vbox1.show_all();
		dlg.get_content_area()->pack_start(vbox1,Gtk::PACK_SHRINK,8);

		int result = dlg.run();
		switch(result){
		case ID_REMOVE:
			{
				std::cout << "RESPONSE Remove" << std::endl;
				m_liste_names[id] = "";
				m_liste_modif_flags[id] = false;
				m_liste_sprites[id].reset();
				m_refGdkWindow->invalidate(true);

			}
			break;
		case ID_DELETE:
			{
				std::cout << "RESPONSE Delete" << std::endl;
				fileName = m_liste_names[id];
  				if( remove( fileName.c_str() ) != 0 ){
					std::cout << "Error deleting " << fileName << std::endl;
  				}
				m_liste_names[id] = "";
				m_liste_modif_flags[id] = false;
				m_liste_sprites[id].reset();
				m_refGdkWindow->invalidate(true);

			}
			break;
		case Gtk::RESPONSE_CANCEL:
			{
				std::cout << "RESPONSE CANCEL" << std::endl;
			}
			break;
		}

	}


}

void spriteArea::on_new_sprite(Glib::RefPtr<Gdk::Pixbuf> newSprite)
{
	//--------------------------------------------
	SetSprite( newSprite);
	m_refGdkWindow->invalidate(true);
}