; Higher level API thoughts, maybe useful later.  Really just a quick note.

;+
; PROCEDURE:
;   das2_load_data
;
; PURPOSE:
;   List all das2 servers know to a single server:
;     IDL> das2_load_data, /peers, server='http://planet.physics.uiowa.edu/das/das2Server'
;     [
;       {
;         "NAME": "Jupiter",
;         "URL": "http://jupiter.physics.uiowa.edu/das/server",
;         "DESCRIPTION": "Off-loads Juno processing from the central server, planet."
;       },
;       {
;         "NAME": "Saturn - IAPCZ",
;         "URL": "http://ipecman.ufa.cas.cz/das/server",
;         "DESCRIPTION": "Institute of Atmospheric Physics, Academy of Sciences of Czechia"
;       },
;       {
;         "NAME": "VO-Paris",
;         "URL": "http://voparis-maser-das.obspm.fr/das2/server",
;         "DESCRIPTION": "Paris Astronomical Data Center"
;       }
;     ]
;   
;   List all data sources available on a single server:
;     IDL> das2_load_data, /datasets, server='http://jupiter.physics.uiowa.edu/das/server'
;     [
;       {
;         "NAME": "Cassini/Ephemeris/Dione_CoRotation",
;         "DESCRIPTION": "Dione centered Cassini location - Cartesian Co-Rotational"
;       },
;       {
;         "NAME": "Galileo/PWS/Survey_Electric",
;         "DESCRIPTION": "Galileo PWS LRS 152-channel calibrated electric (even)"
;       }
;		  ...
;     ]
;
;    List query parameters for a single data source:
;      IDL> server='http://jupiter.physics.uiowa.edu/das/server'
;      IDL> das2_load_data, /info, dataset='Galileo/PWS/Survey_Electric' server=server
;      {
;         "DESCRIPTION": "Galileo PWS LRS 152-channel calibrated electric even",
;         "SCICONTACT": "Bill Kurth <william-kurth@uiowa.edu>",
;         "TECHCONTACT": "Joe Groene <joseph-groene@uiowa.edu>",
;         "VALIDRANGE": ["1996-05-25","2002-11-05"],
;         "EXAMPLES":  [
;            {
;              "DESCRIPTION" : "Orbit Insertion",
;              "RANGE":  ["1996-179T06:15","1996-179T06:45"]
;            }
;          ]
;      }
;
;     Query for data
				  
