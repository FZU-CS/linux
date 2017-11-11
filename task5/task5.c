#include <gtk/gtk.h>
#include <stdlib.h>

int count = 1;
char s1[20], s2[20], s3[20], s4[100];

// interger to string
void get(char *s, long long t) {
	int len = 0;
	
	do {
		s[len++] = t%10+'0';
		t /= 10;
	} while(t != 0);
	
	s[len] = 0;
	for (int l = 0, r = len-1; l < r; l++, r--) {
		char tmp = s[l];
		s[l] = s[r];
		s[r] = tmp;
	}
}

// s4 = s1+s2+s3
void gs4(char *s1, char *s2, char *s3, char *s4) {
	int len = 0;
	for (int i = 0; s1[i]; i++)
		s4[len++] = s1[i];
	s4[len++] = '+';
	
	for (int i = 0; s2[i]; i++)
		s4[len++] = s2[i];
	s4[len++] = '=';
	
	for (int i = 0; s3[i]; i++)
		s4[len++] = s3[i];
	s4[len] = 0;
}

GtkWidget *label;

void on_button_clicked (GtkWidget *button, gpointer userdata) {
	int a = rand()%100, b = rand()%100;
	long long c = (long long)a+b;
	
	get(s1, a);
	get(s2, b);
	get(s3, c);
	gs4(s1, s2, s3, s4);
	gtk_label_set_text(GTK_LABEL(label), s4);
}

int main (int argc , char* argv[]) {
	GtkWidget *box;
	GtkWidget *window;
	GtkWidget *button;

	// initial the window
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_title(GTK_WINDOW(window), "Random");
	gtk_window_set_default_size(GTK_WINDOW(window), 1000, 750);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(window), 20);

	// initial the container
	box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), box);

	// initial the label and button
	label = gtk_label_new("yoo");
	button = gtk_button_new_with_label("Generate");

	// add the label and button to the container
	gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 3);
	gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 3);

	// click the button
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_button_clicked), (gpointer)count);
	gtk_widget_show_all(window);

	gtk_main();
	
	return FALSE;
}