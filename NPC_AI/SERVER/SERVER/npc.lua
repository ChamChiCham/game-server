myid = 99999;

function set_uid(x)
   myid = x;
end

function event_player_move(player)
   player_x = API_get_x(player);
   player_y = API_get_y(player);
   my_x = API_get_x(myid);
   my_y = API_get_y(myid);
   if (player_x == my_x) then
      if (player_y == my_y) then
         API_SendMessage(myid, player, "HELLO");
         for i=1,3 do
            API_sleep(1000);
            API_move_random_dir(myid);
         end
         API_SendMessage(myid, player, "BYE");
      end
   end
end
