/*
 * WSend option - Helps you send various chat entities to your buddies.
 * Copyright (C) 2013 Pooja Ahuja <ahuja.pooja22@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */
#include "internal.h"
#include "version.h"

/* This file defines PURPLE_PLUGINS and includes all the libpurple headers */
#include "purple.h"


static void
send_online_buddy_cb (PurpleBlistNode *cnode, gpointer data)
{
	PurplePlugin *plugin = (PurplePlugin *)data;
	PurpleConnection *gc;
   GSList *blist;
   PurpleBlistNode *gnode, *node;
   PurpleBuddy *b, *buddy;
   PurpleGroup *g, *check_group;
   const char *name, *gname, *receiver;
   int count = 0;
   PurpleAccount *account;
	PurpleIMConversation *im;
	gchar *buddylist = NULL;
	int total, flag = 0, counter = 0;
	gchar *group_info;

   const gchar *groupname = purple_group_get_name(data);
	purple_debug_info("sending_maniac", "The group selected is: %s", groupname);

   buddy = (PurpleBuddy *)cnode;
	purple_debug_info("sending_maniac", "Inside send_online_buddy_cb");
	receiver = purple_buddy_get_name(buddy); 
	purple_debug_info("sending_maniac", "Receiver name: %s", receiver);
	account = purple_buddy_get_account(buddy);
	gc = purple_account_get_connection(account);

	im = purple_conversations_find_im_with_account(receiver, purple_connection_get_account(gc));
       if(im)
	      purple_debug_info("sending_maniac", "Yayy, IM!");
       else
         purple_debug_info("sending_maniac", "Boo! No IM :(");
	if (im == NULL)
      im = purple_im_conversation_new(purple_connection_get_account(gc), receiver);

   for(gnode = purple_blist_get_buddy_list()->root;
       gnode != NULL;
       gnode = gnode->next)
   {
	    if (PURPLE_IS_GROUP(gnode)) {
         g = (PurpleGroup*)gnode; 
		 /*fishy. It says g may be uninitiliazed here. What can I initilalie g to in the else, to get rid of the warning*/  	
         if (g != data)
			   continue;
			else
            break;
       }
	}

   gname = purple_group_get_name(g);
	total = purple_counting_node_get_online_count(PURPLE_COUNTING_NODE(g));;
   purple_debug_info("sending_maniac", "%s (%d): \n",gname, total);
   group_info = g_strdup_printf("%s (%d):\n",gname, total);
	buddylist = g_strconcat(group_info, "\t","\t", NULL);
			 
   for (blist = purple_blist_get_buddies(); 
	  	  blist != NULL;
		  blist = blist->next)
   {
       node = blist->data;

       if (PURPLE_IS_BUDDY(node)) {
		 b = (PurpleBuddy*)node;
       name = purple_buddy_get_alias(b);                  
       check_group = purple_buddy_get_group(b);
                  
       if (data == check_group && PURPLE_BUDDY_IS_ONLINE(b)) {
          ++count;
			 ++counter;

	       if (flag == 1 && counter != total) {
             buddylist = g_strconcat(name, ",\t", NULL);
             flag = 0;
          } else if (flag == 1 && counter == total) {
             buddylist = g_strconcat(name, ".\t", NULL);
				 purple_debug_info(NULL, "%s", buddylist);	
             purple_conversation_send(PURPLE_CONVERSATION(im), buddylist); 
				 buddylist = NULL;
				 count = 0;
			 } else{
				 if (count <= 20 && counter != total) {
				  buddylist = g_strconcat(buddylist, name, ",\t", NULL);	/*fishy cant insert newline*/	
				 } else {
					if (counter == total)
						 buddylist = g_strconcat(buddylist, name, ".\t", NULL);
					else
                   buddylist = g_strconcat(buddylist, name, ",\t", NULL);
						 
                   purple_debug_info(NULL, "%s", buddylist);	
                   purple_conversation_send(PURPLE_CONVERSATION(im), buddylist); 
						 buddylist = NULL;
						 count = 0;
						 flag = 1;
             }
	       }
          }
	    }
	}
   purple_debug_info("sending_maniac", "The counter is (%d): \n",counter);
   
   purple_notify_message (plugin, PURPLE_NOTIFY_MSG_INFO,
		    "Send online buddies", "Successfully sent your online buddies :)", NULL, NULL,
		     NULL, NULL);
	
	g_free(buddylist);
}

static void
send_group_list_cb (PurpleBlistNode *cnode, gpointer data)
{
	PurplePlugin *plugin = (PurplePlugin *)data;
	PurpleBuddy *buddy;
	const char *receiver, *gname = NULL;
	PurpleAccount *account;
	PurpleIMConversation *im;
	PurpleConnection *gc;
	PurpleBlistNode *gnode;
	PurpleGroup *g;
	gchar *grouptotal, *grouplist = NULL;
	int count = 0, counter = 0, flag = 0, total = 0;

	buddy = (PurpleBuddy *)cnode;
	purple_debug_info("sending_maniac", "Inside send_group_list_cb");
	receiver = purple_buddy_get_name(buddy); 
	purple_debug_info("sending_maniac", "Receiver name: %s", receiver);
	account = purple_buddy_get_account(buddy);
	gc = purple_account_get_connection(account);

	im = purple_conversations_find_im_with_account(receiver, purple_connection_get_account(gc));
       if(im)
	      purple_debug_info("sending_maniac", "Yayy, IM!");
       else
         purple_debug_info("sending_maniac", "Boo! No IM :(");
	if (im == NULL)
      im = purple_im_conversation_new(purple_connection_get_account(gc), receiver);

	for(gnode = purple_blist_get_buddy_list()->root;
       gnode != NULL;
       gnode = gnode->next)
   {
	    if (PURPLE_IS_GROUP(gnode)) {
          ++total;
			 grouptotal = g_strdup_printf("Groups (%d):\n", total);
			 grouplist = g_strconcat(grouptotal, gname, ",\t", NULL);
       }
	}

	for(gnode = purple_blist_get_buddy_list()->root;
       gnode != NULL;
       gnode = gnode->next)
   {
	    if (PURPLE_IS_GROUP(gnode)) {
         g = (PurpleGroup*)gnode;
			gname = purple_group_get_name(g);
			purple_debug_info("sending_maniac", " Group checked: %s \n",gname);
			++count;
			++counter;

	       if (flag == 1 && counter != total) {
             grouplist = g_strconcat(gname, ",\t", NULL);
             flag = 0;
          } else if (flag == 1 && counter == total) {
             grouplist = g_strconcat(gname, ".\t", NULL);
				 purple_debug_info(NULL, "%s", grouplist);	
             purple_conversation_send(PURPLE_CONVERSATION(im), grouplist); 
				 grouplist = NULL;
				 count = 0;
			 } else{
				 if (count <= 20 && counter != total) {
				  grouplist = g_strconcat(grouplist, gname, ",\t", NULL);	/*fishy cant insert newline*/	
				 } else {
					if (counter == total)
						 grouplist = g_strconcat(grouplist, gname, ".\t", NULL);
					else
                   grouplist = g_strconcat(grouplist, gname, ",\t", NULL);
						 
                   purple_debug_info(NULL, "%s", grouplist);	
                   purple_conversation_send(PURPLE_CONVERSATION(im), grouplist); 
						 grouplist = NULL;
						 count = 0;
						 flag = 1;
             }
	       }
          
		 }
	}
   purple_notify_message (plugin, PURPLE_NOTIFY_MSG_INFO,
		    "Send group list", "Successfully sent your group list :)", NULL, NULL,
		     NULL, NULL);
	
	g_free(grouplist);	
} 

static void
send_buddy_list_cb (PurpleBlistNode *cnode, gpointer data) 
{
	PurplePlugin *plugin = (PurplePlugin *)data;
	PurpleConnection *gc;
   GSList *blist;
   PurpleBlistNode *gnode, *node;
   PurpleBuddy *b, *buddy;
   PurpleGroup *g, *check_group;
   const char *name, *gname, *receiver;
   int count = 0;
   PurpleAccount *account;
	PurpleIMConversation *im;
	gchar *buddylist = NULL;
	int total, flag = 0, counter = 0;
	gchar *group_info;

   const gchar *groupname = purple_group_get_name(data);
	purple_debug_info("sending_maniac", "The group selected is: %s", groupname);

   buddy = (PurpleBuddy *)cnode;
	purple_debug_info("sending_maniac", "Isnide send_buddy_list_cb");
	receiver = purple_buddy_get_name(buddy); 
	purple_debug_info("sending_maniac", "Receiver name: %s", receiver);
	account = purple_buddy_get_account(buddy);
	gc = purple_account_get_connection(account);

	im = purple_conversations_find_im_with_account(receiver, purple_connection_get_account(gc));
       if(im)
	      purple_debug_info("sending_maniac", "Yayy, IM!");
       else
         purple_debug_info("sending_maniac", "Boo! No IM :(");
	if (im == NULL)
      im = purple_im_conversation_new(purple_connection_get_account(gc), receiver);

   for(gnode = purple_blist_get_buddy_list()->root;
       gnode != NULL;
       gnode = gnode->next)
   {
	    if (PURPLE_IS_GROUP(gnode)) {
         g = (PurpleGroup*)gnode; 
		 /*fishy. It says g may be uninitiliazed here. What can I initilalie g to in the else, to get rid of the warning*/  	
         if (g != data)
			   continue;
			else
            break;
       }
	}

   gname = purple_group_get_name(g);
	total = purple_counting_node_get_current_size(PURPLE_COUNTING_NODE(g));
   purple_debug_info("sending_maniac", "%s (%d): \n",gname, total);
   group_info = g_strdup_printf("%s (%d):\n",gname, total);
	buddylist = g_strconcat(group_info, "\t","\t", NULL);
			 
   for (blist = purple_blist_get_buddies(); 
	  	  blist != NULL;
		  blist = blist->next)
   {
       node = blist->data;

       if (PURPLE_IS_BUDDY(node)) {
		 b = (PurpleBuddy*)node;
       name = purple_buddy_get_alias(b);                  
       check_group = purple_buddy_get_group(b);
                  
       if (data == check_group) {
          ++count;
			 ++counter;

	       if (flag == 1 && counter != total) {
             buddylist = g_strconcat(name, ",\t", NULL);
             flag = 0;
          } else if (flag == 1 && counter == total) {
             buddylist = g_strconcat(name, ".\t", NULL);
				 purple_debug_info(NULL, "%s", buddylist);	
             purple_conversation_send(PURPLE_CONVERSATION(im), buddylist); 
				 buddylist = NULL;
				 count = 0;
			 } else{
				 if (count <= 20 && counter != total) {
				  buddylist = g_strconcat(buddylist, name, ",\t", NULL);	/*fishy cant insert newline*/	
				 } else {
					if (counter == total)
						 buddylist = g_strconcat(buddylist, name, ".\t", NULL);
					else
                   buddylist = g_strconcat(buddylist, name, ",\t", NULL);
						 
                   purple_debug_info(NULL, "%s", buddylist);	
                   purple_conversation_send(PURPLE_CONVERSATION(im), buddylist); 
						 buddylist = NULL;
						 count = 0;
						 flag = 1;
             }
	       }
          }
	    }
	}
   purple_debug_info("sending_maniac", "The counter is (%d): \n",counter);
   
   purple_notify_message (plugin, PURPLE_NOTIFY_MSG_INFO,
		    "Send buddies", "Successfully sent your buddy list :)", NULL, NULL,
		     NULL, NULL);
	
	g_free(buddylist);
}

static void
extended_menu_cb(PurpleBlistNode *node, GList **menu, gpointer data)
{
	GList *submenu = NULL, *submenu_1 = NULL, *submenu_2 = NULL;
	PurpleMenuAction *action = NULL;
	PurpleBlistNode *group;
	PurpleMenuAction *action_submenu = NULL, *action_submenu_1 = NULL, *action_submenu_2 = NULL; 
	const gchar *group_name = NULL;

	if (!PURPLE_IS_BUDDY(node))
		return;

   for (group = purple_blist_get_root(); group; group = purple_blist_node_get_sibling_next(group)) {
		if (!PURPLE_IS_GROUP(group))
			continue;
			
		group_name = purple_group_get_name((PurpleGroup *)group);

		action_submenu_1 = purple_menu_action_new(group_name,
		                 PURPLE_CALLBACK(send_buddy_list_cb),
		                 group, NULL); 
		submenu_1 = g_list_append (submenu_1, action_submenu_1);

	}

	for (group = purple_blist_get_root(); group; group = purple_blist_node_get_sibling_next(group)) {
		if (!PURPLE_IS_GROUP(group))
			continue;
			
		group_name = purple_group_get_name((PurpleGroup *)group);

		action_submenu_2 = purple_menu_action_new(group_name,
		                 PURPLE_CALLBACK(send_online_buddy_cb),
		                 group, NULL); 
		submenu_2 = g_list_append (submenu_2, action_submenu_2);

	}

   action_submenu = purple_menu_action_new(_("Group list"), 
		                                PURPLE_CALLBACK(send_group_list_cb), NULL, NULL);
	submenu = g_list_append (submenu, action_submenu);
   action_submenu = purple_menu_action_new(_("Buddy List for"), 
		                                NULL, NULL, submenu_1);
	submenu = g_list_append (submenu, action_submenu);
	action_submenu = purple_menu_action_new(_("Online Buddies for"), 
		                                NULL, NULL, submenu_2);
	submenu = g_list_append (submenu, action_submenu);

	action = purple_menu_action_new(_("Send"), 
		                                NULL, NULL, submenu);
	*menu = g_list_append (*menu, action);
}

static gboolean
plugin_load (PurplePlugin * plugin)
{
	purple_signal_connect(purple_blist_get_handle(), "blist-node-extended-menu",
						plugin, PURPLE_CALLBACK(extended_menu_cb), NULL);

	return TRUE;
}

static PurplePluginInfo info = {
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,
	NULL,
	0,
	NULL,
	PURPLE_PRIORITY_DEFAULT,

	"core-send_option",
	"Send Chat Entities!",
	DISPLAY_VERSION,

	"Sending Maniac",
	"This helps you send varoius chat entities.",
	"Pooja Ahuja <ahuja.pooja22@gmail.com>",        /*author */
	"http://ahujapooja.com",


	plugin_load,
	NULL,                                           /**< unload */
	NULL,                                           /**< destroy */
	NULL,                                           /**< ui_info */
	NULL,                                           /**< extra_info */
	NULL,                                           /**< prefs_info */
	NULL,                                           /**< actions */

	/* padding */
	NULL,
	NULL,
	NULL,
	NULL
};

static void
init_plugin (PurplePlugin * plugin)
{
}

PURPLE_INIT_PLUGIN (hello_world, init_plugin, info)
