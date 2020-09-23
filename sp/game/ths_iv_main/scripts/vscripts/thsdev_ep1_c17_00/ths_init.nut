//********************************************************************************************
//ths_init.nut is called on newgame or transitions
//********************************************************************************************
printl("==== calling ths_init.nut")
printl("==== [THS] inc 2020.")
printl("==== [THS] Loading...")
printl("==== [THS] Starting map - thsdev_ep1_c17_00")
printl("==== [THS] Loading Successful!!!")


function tonemap_main_settings_fire()
{
 EntFire (EntityGroup[0].GetName(), "SetAutoExposureMax", 4)
 EntFire (EntityGroup[0].GetName(), "SetAutoExposureMin", 0.5)
 EntFire (EntityGroup[0].GetName(), "SetTonemapRate", 0.25)
 EntFire (EntityGroup[0].GetName(), "SetBloomScale", 0.72)
 //thsdev_test_send...
 EntFire (EntityGroup[0].GetName(), "FireUser1")
 printl("==== tonemap_main_settings_fire_send...")
}

function tonemap_main_settings_testdone()
{
 printl("==== tonemap_main_settings_testdone!!!")
}