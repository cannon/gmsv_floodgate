require("floodgate")

--At most 500 messages can be sent per second
timer.Create("Floodgate",1,0,function()
	ConsoleFloodgate(500)
end)