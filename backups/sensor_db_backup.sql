--
-- PostgreSQL database dump
--

\restrict 1gAiU2I9YfOTqAvqq8IGXVbYL3YUpW40wm9FCqdAZ0ESBX3kGKCosZrZXLGRp88

-- Dumped from database version 18.0
-- Dumped by pg_dump version 18.0

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET transaction_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: audit_log; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.audit_log (
    log_id bigint NOT NULL,
    user_id integer,
    username character varying(50),
    action character varying(100) NOT NULL,
    entity_type character varying(50),
    entity_id character varying(50),
    old_values jsonb,
    new_values jsonb,
    ip_address inet,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.audit_log OWNER TO postgres;

--
-- Name: audit_log_log_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.audit_log_log_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.audit_log_log_id_seq OWNER TO postgres;

--
-- Name: audit_log_log_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.audit_log_log_id_seq OWNED BY public.audit_log.log_id;


--
-- Name: object_types; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.object_types (
    type_id integer NOT NULL,
    type_code character varying(50) NOT NULL,
    type_name character varying(100) NOT NULL,
    can_have_children boolean DEFAULT false,
    icon_name character varying(50),
    color_code character varying(7),
    svg_icon_path text NOT NULL
);


ALTER TABLE public.object_types OWNER TO postgres;

--
-- Name: object_types_type_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.object_types_type_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.object_types_type_id_seq OWNER TO postgres;

--
-- Name: object_types_type_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.object_types_type_id_seq OWNED BY public.object_types.type_id;


--
-- Name: objects; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.objects (
    object_id character varying(50) NOT NULL,
    object_type_id integer,
    parent_object_id character varying(50),
    name character varying(100) NOT NULL,
    description text,
    position_x numeric(6,2),
    position_y numeric(6,2),
    size_width numeric(6,2),
    size_height numeric(6,2),
    svg_scheme_path text NOT NULL,
    status character varying(20) DEFAULT 'active'::character varying,
    created_at timestamp with time zone
);


ALTER TABLE public.objects OWNER TO postgres;

--
-- Name: schema_migrations; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.schema_migrations (
    version integer NOT NULL,
    applied_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.schema_migrations OWNER TO postgres;

--
-- Name: sensor_readings; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.sensor_readings (
    reading_id bigint NOT NULL,
    sensor_id character varying(50) NOT NULL,
    value numeric(10,3) NOT NULL,
    "timestamp" timestamp with time zone
);


ALTER TABLE public.sensor_readings OWNER TO postgres;

--
-- Name: sensor_readings_reading_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.sensor_readings_reading_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.sensor_readings_reading_id_seq OWNER TO postgres;

--
-- Name: sensor_readings_reading_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.sensor_readings_reading_id_seq OWNED BY public.sensor_readings.reading_id;


--
-- Name: sensor_types; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.sensor_types (
    type_id integer NOT NULL,
    type_code character varying(20) NOT NULL,
    type_name character varying(50) NOT NULL,
    unit character varying(10),
    color_code character varying(7),
    icon_name character varying(50),
    svg_image_path text
);


ALTER TABLE public.sensor_types OWNER TO postgres;

--
-- Name: sensor_types_type_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.sensor_types_type_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.sensor_types_type_id_seq OWNER TO postgres;

--
-- Name: sensor_types_type_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.sensor_types_type_id_seq OWNED BY public.sensor_types.type_id;


--
-- Name: sensors; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.sensors (
    sensor_id character varying(50) NOT NULL,
    type_id integer,
    object_id character varying(50) NOT NULL,
    name character varying(100) NOT NULL,
    description text,
    position_x numeric(5,2),
    position_y numeric(5,2),
    status character varying(20) DEFAULT 'active'::character varying,
    last_value numeric(10,3),
    last_update timestamp with time zone,
    install_date timestamp with time zone,
    protocol character varying DEFAULT 'snmp'::character varying,
    host character varying,
    port integer DEFAULT 161,
    community character varying DEFAULT 'public'::character varying,
    oid character varying,
    modbus_address integer DEFAULT 1,
    modbus_register integer DEFAULT 0,
    poll_interval integer DEFAULT 5000,
    enabled boolean DEFAULT true,
    command character varying,
    terminator character varying DEFAULT 'CR'::character varying,
    regex character varying,
    checksum_type character varying DEFAULT 'None'::character varying,
    polling_interval integer DEFAULT 5000,
    is_active boolean DEFAULT true,
    connection_params text
);


ALTER TABLE public.sensors OWNER TO postgres;

--
-- Name: system_settings; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.system_settings (
    setting_id integer NOT NULL,
    setting_key character varying(100) NOT NULL,
    setting_value text,
    description text,
    updated_by integer,
    updated_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.system_settings OWNER TO postgres;

--
-- Name: system_settings_setting_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.system_settings_setting_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.system_settings_setting_id_seq OWNER TO postgres;

--
-- Name: system_settings_setting_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.system_settings_setting_id_seq OWNED BY public.system_settings.setting_id;


--
-- Name: user_sessions; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_sessions (
    session_id integer NOT NULL,
    user_id integer,
    token character varying(255) NOT NULL,
    expires_at timestamp without time zone NOT NULL,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.user_sessions OWNER TO postgres;

--
-- Name: user_sessions_session_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.user_sessions_session_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.user_sessions_session_id_seq OWNER TO postgres;

--
-- Name: user_sessions_session_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.user_sessions_session_id_seq OWNED BY public.user_sessions.session_id;


--
-- Name: users; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.users (
    user_id integer NOT NULL,
    username character varying(50) NOT NULL,
    password_hash character varying(255) NOT NULL,
    email character varying(100) NOT NULL,
    role character varying(20) DEFAULT 'user'::character varying,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    last_login timestamp without time zone
);


ALTER TABLE public.users OWNER TO postgres;

--
-- Name: users_user_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.users_user_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.users_user_id_seq OWNER TO postgres;

--
-- Name: users_user_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.users_user_id_seq OWNED BY public.users.user_id;


--
-- Name: audit_log log_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.audit_log ALTER COLUMN log_id SET DEFAULT nextval('public.audit_log_log_id_seq'::regclass);


--
-- Name: object_types type_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.object_types ALTER COLUMN type_id SET DEFAULT nextval('public.object_types_type_id_seq'::regclass);


--
-- Name: sensor_readings reading_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sensor_readings ALTER COLUMN reading_id SET DEFAULT nextval('public.sensor_readings_reading_id_seq'::regclass);


--
-- Name: sensor_types type_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sensor_types ALTER COLUMN type_id SET DEFAULT nextval('public.sensor_types_type_id_seq'::regclass);


--
-- Name: system_settings setting_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.system_settings ALTER COLUMN setting_id SET DEFAULT nextval('public.system_settings_setting_id_seq'::regclass);


--
-- Name: user_sessions session_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_sessions ALTER COLUMN session_id SET DEFAULT nextval('public.user_sessions_session_id_seq'::regclass);


--
-- Name: users user_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users ALTER COLUMN user_id SET DEFAULT nextval('public.users_user_id_seq'::regclass);


--
-- Data for Name: audit_log; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.audit_log (log_id, user_id, username, action, entity_type, entity_id, old_values, new_values, ip_address, created_at) FROM stdin;
\.


--
-- Data for Name: object_types; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.object_types (type_id, type_code, type_name, can_have_children, icon_name, color_code, svg_icon_path) FROM stdin;
1	complex	Промышленный комплекс	t	city	#333333	/icons/complex.svg
2	building	Здание	t	building	#FF6B6B	/icons/building.svg
3	room	Помещение	f	door-open	#2196F3	/icons/room.svg
\.


--
-- Data for Name: objects; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.objects (object_id, object_type_id, parent_object_id, name, description, position_x, position_y, size_width, size_height, svg_scheme_path, status, created_at) FROM stdin;
rscc_zeleznogorsk	1	\N	ЦКС "Железногорск"	\N	972.00	806.00	120.00	60.00	/schemes/antenna.svg	active	\N
rscc_habarovsk	1	\N	ЦКС "Хабаровск"	\N	1636.00	865.00	100.00	60.00	/schemes/antenna.svg	active	\N
room_101	3	plan_mo	Кабинет 101	\N	10.00	10.00	30.00	20.00	/schemes/room_101.svg	active	\N
plan_mo	2	rscc_bearlakes	ЦКС "Медвежьи Озёра"	\N	20.00	30.00	0.00	0.00	/schemes/plan_mo.svg	active	\N
rscc_bearlakes	1	\N	ЦКС "Медвежьи Озёра"	\N	293.00	494.00	140.00	60.00	/schemes/antenna.svg	active	\N
rscc_skolkovo	1	\N	ЦКС "Сколково"	\N	269.00	491.00	100.00	60.00	/schemes/antenna.svg	active	\N
rscc_vladimir	1	\N	ЦКС "Владимир"	\N	315.00	523.00	100.00	60.00	/schemes/antenna.svg	active	\N
rscc_dubna	1	\N	ЦКС "Дубна"	\N	300.00	479.00	100.00	60.00	/schemes/antenna.svg	active	\N
tc_shabolovka	1	\N	ТЦ "Шаболовка"	\N	280.70	498.00	100.00	60.00	/schemes/antenna_tc.svg	active	\N
\.


--
-- Data for Name: schema_migrations; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.schema_migrations (version, applied_at) FROM stdin;
1	2026-08-26 15:36:15.894093
\.


--
-- Data for Name: sensor_readings; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.sensor_readings (reading_id, sensor_id, value, "timestamp") FROM stdin;
1	temp_101	245625.000	\N
2	temp_101	245625.000	\N
3	temp_101	245625.000	\N
4	temp_101	245625.000	\N
5	temp_101	245625.000	\N
6	temp_101	245625.000	\N
7	hum_101	276250.000	\N
8	temp_101	245625.000	\N
9	hum_101	276250.000	\N
10	temp_101	245625.000	\N
11	hum_101	276250.000	\N
12	temp_101	245625.000	\N
13	hum_101	276250.000	\N
14	temp_101	245625.000	\N
15	hum_101	276250.000	\N
16	temp_101	245625.000	\N
17	hum_101	276875.000	\N
18	temp_101	245625.000	\N
19	hum_101	276875.000	\N
20	temp_101	245625.000	\N
21	hum_101	276875.000	\N
22	temp_101	245625.000	\N
23	hum_101	284375.000	\N
24	temp_101	258750.000	\N
25	hum_101	284375.000	\N
26	temp_101	258750.000	\N
27	hum_101	284375.000	\N
28	temp_101	258750.000	\N
29	hum_101	284375.000	\N
30	temp_101	258750.000	\N
31	hum_101	284375.000	\N
32	temp_101	258750.000	\N
33	hum_101	284375.000	\N
34	temp_101	258750.000	\N
35	hum_101	284375.000	\N
36	temp_101	258750.000	\N
\.


--
-- Data for Name: sensor_types; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.sensor_types (type_id, type_code, type_name, unit, color_code, icon_name, svg_image_path) FROM stdin;
1	TEMP	Температура	°C	#FF9800	thermometer	/images/sensors/temperature.svg
2	HUM	Влажность	%	#2196F3	tint	/images/sensors/humidity.svg
3	CO2	CO2	ppm	#9C27B0	wind	/images/sensors/co2.svg
\.


--
-- Data for Name: sensors; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.sensors (sensor_id, type_id, object_id, name, description, position_x, position_y, status, last_value, last_update, install_date, protocol, host, port, community, oid, modbus_address, modbus_register, poll_interval, enabled, command, terminator, regex, checksum_type, polling_interval, is_active, connection_params) FROM stdin;
temp_101	1	plan_mo	Температура кабинет 101	\N	50.00	50.00	active	\N	\N	2024-01-15 00:00:00+03	snmp	10.60.2.207	161	public	.1.3.6.1.4.1.61858.5.1.2.2.0	1	0	5000	t	\N	CR	\N	None	5000	t	\N
hum_101	2	plan_mo	Влажность кабинет 101	\N	30.00	70.00	active	\N	\N	2024-01-15 00:00:00+03	snmp	10.60.2.207	161	public	.1.3.6.1.4.1.61858.5.1.1.2.0	1	0	5000	t	\N	CR	\N	None	5000	t	\N
\.


--
-- Data for Name: system_settings; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.system_settings (setting_id, setting_key, setting_value, description, updated_by, updated_at) FROM stdin;
\.


--
-- Data for Name: user_sessions; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.user_sessions (session_id, user_id, token, expires_at, created_at) FROM stdin;
1	1	eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VySWQiOjEsInVzZXJuYW1lIjoiYWRtaW4iLCJyb2xlIjoiYWRtaW4iLCJpYXQiOjE3NzQzNDM2NDcsImV4cCI6MTc3NDQzMDA0N30.BSYkPqqyO2GrnUNrwaHnMgwC2go6zlRcZxZh7Zto2P0	2026-03-25 12:14:07.415	2026-03-24 12:14:07.417452
\.


--
-- Data for Name: users; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.users (user_id, username, password_hash, email, role, created_at, last_login) FROM stdin;
1	admin	$2b$10$PdjZ2FBXsH00st.Oto7t6eVGpGlWz3dnUnbmvpiqwZDqVeOLwRAN2	admin@example.com	admin	2026-03-24 12:03:19.395093	2026-03-24 12:14:07.422799
\.


--
-- Name: audit_log_log_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.audit_log_log_id_seq', 1, false);


--
-- Name: object_types_type_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.object_types_type_id_seq', 3, true);


--
-- Name: sensor_readings_reading_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.sensor_readings_reading_id_seq', 36, true);


--
-- Name: sensor_types_type_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.sensor_types_type_id_seq', 3, true);


--
-- Name: system_settings_setting_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.system_settings_setting_id_seq', 1, false);


--
-- Name: user_sessions_session_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.user_sessions_session_id_seq', 1, true);


--
-- Name: users_user_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.users_user_id_seq', 1, true);


--
-- Name: audit_log audit_log_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.audit_log
    ADD CONSTRAINT audit_log_pkey PRIMARY KEY (log_id);


--
-- Name: object_types object_types_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.object_types
    ADD CONSTRAINT object_types_pkey PRIMARY KEY (type_id);


--
-- Name: object_types object_types_type_code_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.object_types
    ADD CONSTRAINT object_types_type_code_key UNIQUE (type_code);


--
-- Name: object_types object_types_type_code_key1; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.object_types
    ADD CONSTRAINT object_types_type_code_key1 UNIQUE (type_code);


--
-- Name: object_types object_types_type_code_key2; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.object_types
    ADD CONSTRAINT object_types_type_code_key2 UNIQUE (type_code);


--
-- Name: objects objects_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.objects
    ADD CONSTRAINT objects_pkey PRIMARY KEY (object_id);


--
-- Name: schema_migrations schema_migrations_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.schema_migrations
    ADD CONSTRAINT schema_migrations_pkey PRIMARY KEY (version);


--
-- Name: sensor_readings sensor_readings_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sensor_readings
    ADD CONSTRAINT sensor_readings_pkey PRIMARY KEY (reading_id);


--
-- Name: sensor_types sensor_types_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sensor_types
    ADD CONSTRAINT sensor_types_pkey PRIMARY KEY (type_id);


--
-- Name: sensor_types sensor_types_type_code_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sensor_types
    ADD CONSTRAINT sensor_types_type_code_key UNIQUE (type_code);


--
-- Name: sensor_types sensor_types_type_code_key1; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sensor_types
    ADD CONSTRAINT sensor_types_type_code_key1 UNIQUE (type_code);


--
-- Name: sensor_types sensor_types_type_code_key2; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sensor_types
    ADD CONSTRAINT sensor_types_type_code_key2 UNIQUE (type_code);


--
-- Name: sensors sensors_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sensors
    ADD CONSTRAINT sensors_pkey PRIMARY KEY (sensor_id);


--
-- Name: system_settings system_settings_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.system_settings
    ADD CONSTRAINT system_settings_pkey PRIMARY KEY (setting_id);


--
-- Name: system_settings system_settings_setting_key_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.system_settings
    ADD CONSTRAINT system_settings_setting_key_key UNIQUE (setting_key);


--
-- Name: user_sessions user_sessions_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_sessions
    ADD CONSTRAINT user_sessions_pkey PRIMARY KEY (session_id);


--
-- Name: user_sessions user_sessions_token_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_sessions
    ADD CONSTRAINT user_sessions_token_key UNIQUE (token);


--
-- Name: users users_email_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT users_email_key UNIQUE (email);


--
-- Name: users users_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT users_pkey PRIMARY KEY (user_id);


--
-- Name: users users_username_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT users_username_key UNIQUE (username);


--
-- Name: idx_objects_parent; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_objects_parent ON public.objects USING btree (parent_object_id);


--
-- Name: idx_sensor_readings_sensor_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_sensor_readings_sensor_id ON public.sensor_readings USING btree (sensor_id);


--
-- Name: idx_sensor_readings_sensor_time; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_sensor_readings_sensor_time ON public.sensor_readings USING btree (sensor_id, "timestamp" DESC);


--
-- Name: idx_sensor_readings_timestamp; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_sensor_readings_timestamp ON public.sensor_readings USING btree ("timestamp");


--
-- Name: idx_sensors_object; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_sensors_object ON public.sensors USING btree (object_id);


--
-- Name: idx_sensors_type; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_sensors_type ON public.sensors USING btree (type_id);


--
-- Name: audit_log audit_log_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.audit_log
    ADD CONSTRAINT audit_log_user_id_fkey FOREIGN KEY (user_id) REFERENCES public.users(user_id) ON DELETE SET NULL;


--
-- Name: objects objects_object_type_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.objects
    ADD CONSTRAINT objects_object_type_id_fkey FOREIGN KEY (object_type_id) REFERENCES public.object_types(type_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: objects objects_parent_object_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.objects
    ADD CONSTRAINT objects_parent_object_id_fkey FOREIGN KEY (parent_object_id) REFERENCES public.objects(object_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: sensor_readings sensor_readings_sensor_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sensor_readings
    ADD CONSTRAINT sensor_readings_sensor_id_fkey FOREIGN KEY (sensor_id) REFERENCES public.sensors(sensor_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: sensors sensors_object_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sensors
    ADD CONSTRAINT sensors_object_id_fkey FOREIGN KEY (object_id) REFERENCES public.objects(object_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: sensors sensors_type_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sensors
    ADD CONSTRAINT sensors_type_id_fkey FOREIGN KEY (type_id) REFERENCES public.sensor_types(type_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: system_settings system_settings_updated_by_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.system_settings
    ADD CONSTRAINT system_settings_updated_by_fkey FOREIGN KEY (updated_by) REFERENCES public.users(user_id);


--
-- Name: user_sessions user_sessions_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_sessions
    ADD CONSTRAINT user_sessions_user_id_fkey FOREIGN KEY (user_id) REFERENCES public.users(user_id) ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

\unrestrict 1gAiU2I9YfOTqAvqq8IGXVbYL3YUpW40wm9FCqdAZ0ESBX3kGKCosZrZXLGRp88

