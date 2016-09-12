// vts values

function include(filename) {
	var head = document.getElementsByTagName('head')[0];
	script = document.createElement('script');
	script.src = filename;
	script.type = 'text/javascript';
	head.appendChild(script);
}

// base location of vts
var locationngxvtstatus = "ngxvtstatus";

// vtsUpdateInterval, 2500=2,5seconds
var vtsUpdateIntervalvv = 2500;

// Streams that are down color in red
var vtsupstreamdown = "background:red; color:white;";

// Waiting connections color in red when higher then trigger value
var vtsconnwaiting = "background:red; color:white;";
var vtsconnwaitingtrigger = 1000;

// Either move the language values here or load it from a parameter js file,
//  possibly add language detection, however, often when the NOC is outsourced
//  the outsource party uses a default EN setting, you are then presenting a
//  language which is not native...
// With more then one NOC is it better to use nginx map feature to rewrite a language.js
//  file based on IP location

// include("vtsvalues-en.js");
// include("vtsvalues-de.js");
