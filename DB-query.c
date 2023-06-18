#include <gtk/gtk.h>
#include <libpq-fe.h>

GtkWidget *entry1;
GtkWidget *entry2;
GtkWidget *status_label;
PGconn *conn = NULL;

void connect_database(GtkWidget *widget, gpointer data) {
    const gchar *dbname = gtk_entry_get_text(GTK_ENTRY(entry1));
    const gchar *username = gtk_entry_get_text(GTK_ENTRY(entry2));
    
    // Connect to the database
    conn = PQconnectdbParams(NULL, "dbname", dbname, "user", username, NULL);
    
    // Check if the connection was successful
    if (PQstatus(conn) != CONNECTION_OK) {
        gchar *error_message = g_strdup_printf("Connection to database failed: %s", PQerrorMessage(conn));
        gtk_label_set_text(GTK_LABEL(status_label), error_message);
        g_free(error_message);
        PQfinish(conn);
        conn = NULL;
        return;
    }
    
    gtk_label_set_text(GTK_LABEL(status_label), "Connected to database");
}

void add_entry(GtkWidget *widget, gpointer data) {
    if (!conn) {
        gtk_label_set_text(GTK_LABEL(status_label), "Not connected to database");
        return;
    }
    
    const gchar *value1 = gtk_entry_get_text(GTK_ENTRY(entry1));
    const gchar *value2 = gtk_entry_get_text(GTK_ENTRY(entry2));
    
    // Prepare the SQL statement
    gchar *sql = g_strdup_printf("INSERT INTO mytable (column1, column2) VALUES ('%s', '%s')", value1, value2);
    
    // Execute the SQL statement
    PGresult *res = PQexec(conn, sql);
    
    // Check if the execution was successful
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        gchar *error_message = g_strdup_printf("Failed to execute SQL statement: %s", PQerrorMessage(conn));
        gtk_label_set_text(GTK_LABEL(status_label), error_message);
        g_free(error_message);
        PQclear(res);
        g_free(sql);
        return;
    }
    
    // Cleanup
    PQclear(res);
    g_free(sql);
    
    gtk_label_set_text(GTK_LABEL(status_label), "Entry added successfully");
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Add Entry");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Create a vertical box container
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    // Create entry fields
    entry1 = gtk_entry_new();
    entry2 = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), entry2, FALSE, FALSE, 0);
    
    // Create connect button
    GtkWidget *connect_button = gtk_button_new_with_label("Connect to Database");
    g_signal_connect(connect_button, "clicked", G_CALLBACK(connect_database), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), connect_button, FALSE, FALSE, 0);
    
    // Create add button
    GtkWidget *add_button = gtk_button_new_with_label("Add Entry");
    g_signal_connect(add_button, "clicked", G_CALLBACK(add_entry), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), add_button, FALSE, FALSE, 0);
    
    // Create status label
    status_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 0);
    
    // Show all widgets
    gtk_widget_show_all(window);
    
    // Start the main loop
    gtk_main();
    
    // Clean up the connection
    if (conn) {
        PQfinish(conn);
    }
    
    return 0;
}