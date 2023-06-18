#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <postgresql/libpq-fe.h>

GtkWidget *entry1, *entry2, *entry3, *entry4, *entry5, *entry6;
GtkWidget *status_label;
PGconn *conn;

void add_entry(GtkWidget *widget, gpointer data)
{
    // Get text from entry fields
    const gchar *name = gtk_entry_get_text(GTK_ENTRY(entry1));
    const gchar *age = gtk_entry_get_text(GTK_ENTRY(entry2));
    const gchar *email = gtk_entry_get_text(GTK_ENTRY(entry3));
    const gchar *weight = gtk_entry_get_text(GTK_ENTRY(entry4));
    const gchar *origin = gtk_entry_get_text(GTK_ENTRY(entry5));
    const gchar *destination = gtk_entry_get_text(GTK_ENTRY(entry6));

    // Construct the SQL query
    char query[500];
    sprintf(query, "INSERT INTO passengers (name, age, email, weight, origin, destination) VALUES ('%s', '%s', '%s', '%s', '%s', '%s')",
            name, age, email, weight, origin, destination);

    // Execute the query
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) == PGRES_COMMAND_OK)
    {
        // Set status label text
        gtk_label_set_text(GTK_LABEL(status_label), "Entry added successfully");
    }
    else
    {
        // Set status label text with error message
        gtk_label_set_text(GTK_LABEL(status_label), PQerrorMessage(conn));
    }

    // Clear entry fields
    gtk_entry_set_text(GTK_ENTRY(entry1), "");
    gtk_entry_set_text(GTK_ENTRY(entry2), "");
    gtk_entry_set_text(GTK_ENTRY(entry3), "");
    gtk_entry_set_text(GTK_ENTRY(entry4), "");
    gtk_entry_set_text(GTK_ENTRY(entry5), "");
    gtk_entry_set_text(GTK_ENTRY(entry6), "");

    // Cleanup
    PQclear(res);
}

int main(int argc, char *argv[])
{
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Passenger Entry");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box container
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create a grid
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);

    // Create labels and entry fields
    GtkWidget *label1 = gtk_label_new("Name:");
    entry1 = gtk_entry_new();
    GtkWidget *label2 = gtk_label_new("Age:");
    entry2 = gtk_entry_new();
    GtkWidget *label3 = gtk_label_new("Email:");
    entry3 = gtk_entry_new();
    GtkWidget *label4 = gtk_label_new("Weight:");
    entry4 = gtk_entry_new();
    GtkWidget *label5 = gtk_label_new("Origin:");
    entry5 = gtk_entry_new();
    GtkWidget *label6 = gtk_label_new("Destination:");
    entry6 = gtk_entry_new();

    gtk_grid_attach(GTK_GRID(grid), label1, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry1, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label2, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry2, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label3, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry3, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label4, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry4, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label5, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry5, 1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label6, 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry6, 1, 5, 1, 1);

    // Create a button
    GtkWidget *button = gtk_button_new_with_label("Add Entry");
    g_signal_connect(button, "clicked", G_CALLBACK(add_entry), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

    // Create a status label
    status_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 0);

    // Show all widgets
    gtk_widget_show_all(window);

    // Connect to the database
    conn = PQconnectdb("dbname=mydatabase user=myuser password=mypassword");

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        return 1;
    }

    // Start the main GTK loop
    gtk_main();

    // Disconnect from the database
    PQfinish(conn);

    return 0;
}
