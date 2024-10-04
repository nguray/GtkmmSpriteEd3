/*-------------------------------------------------------*\
  Compilation doit être fait dans la console Msys64 :
  Allez dans la répertoire du projet et lancer :
     mingw32-make.exe -f Makefile all
\*-------------------------------------------------------*/
//foo.cpp
#include "myApplication.h"
#include <gtkmm/application.h>
#include <gtkmm/cssprovider.h>
#include <iostream>

#define GTK_STYLE_PROVIDER_PRIORITY_APPLICATION 600

Glib::RefPtr<myApplication>  app;

int main (int argc, char *argv[])
{
  //------------------------------------------------------------------      
  //app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
  app = myApplication::create();

  //--
  //Glib::RefPtr<Gtk::CssProvider> cssProvider = Gtk::CssProvider::create();
  //cssProvider->load_from_path("style.css");
  //Glib::RefPtr<Gtk::StyleContext> styleContext = Gtk::StyleContext::create();
  //get default screen
  //Glib::RefPtr<Gdk::Screen> screen = Gdk::Screen::get_default();
  //add provider for screen in all application
  //styleContext->add_provider_for_screen(screen, cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  //mainWindow mainW;

  //Shows the window and returns when it is closed.
  //return app->run(mainW);
  return app->run(argc, argv);


}